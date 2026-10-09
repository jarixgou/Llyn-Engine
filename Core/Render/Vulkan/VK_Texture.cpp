#include "VK_Texture.h"

#include "VK_TextureInfo.h"

#include "VK_Buffer.h"
#include "VK_Device.h"
#include "VK_VmaAllocatorWrapper.h"
#include "../../Logger.h"

void VK_Texture::Init(VK_TextureInfo& _info)
{
	const uint32_t mipLevels = static_cast<uint32_t>(floor(log2(std::max(_info.textureSize.x, _info.textureSize.y))) + 1);
	const uint32_t actualMipLevels = _info.config.mipmap ? mipLevels : 1;

	const VkFormat actualFormat = _info.preferredFormat != VK_FORMAT_UNDEFINED ?
		_info.preferredFormat : GetFormat(_info.config.format, _info.config.sRGB);

	VK_Image::Init(actualFormat, _info.layout, _info.config.type, _info.aspect, _info.usage, actualMipLevels, _info.textureSize);

	if (_info.data != nullptr)
	{
		const VkDeviceSize imageSizeByte = _info.textureSize.x * _info.textureSize.y * 4;
		VK_Buffer stagingBuff = VK_Buffer::CreateStagingBuffer(_info.data, imageSizeByte, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		VkCommandBuffer cmdBuff = VK_Buffer::BeginSingleTimeCommand();

		TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VK_Buffer::CopyBufferToImage(cmdBuff, &stagingBuff, &m_image, _info.textureSize);

		if (_info.config.mipmap)
		{
			GenerateMipMaps(cmdBuff, _info.textureSize);
		}
		else
		{
			TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		VK_Buffer::EndSingleTimeCommand(cmdBuff);
	}
	CreateSampler(GetFilter(_info.config.filter), GetMipFilter(_info.config.mipMapFilter), GetWrapMode(_info.config.wrapMode));
}

void VK_Texture::Cleanup()
{
	vkDestroySampler(*VK_Device::Get().GetDevice(), m_sampler, VK_NULL_HANDLE);

	VK_Image::Cleanup();
}

void VK_Texture::CreateSampler(VkFilter _filter, VkSamplerMipmapMode _mipFilter,
                               VkSamplerAddressMode _wrapMode)
{
	VkPhysicalDeviceProperties physicDeviceProps{};
	vkGetPhysicalDeviceProperties(*VK_Device::Get().GetPhycicalDevice(), &physicDeviceProps);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = VK_NULL_HANDLE;
	samplerInfo.magFilter = _filter;
	samplerInfo.minFilter = _filter;
	samplerInfo.mipmapMode = _mipFilter;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
	samplerInfo.addressModeU = _wrapMode;
	samplerInfo.addressModeV = _wrapMode;
	samplerInfo.addressModeW = _wrapMode;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = physicDeviceProps.limits.maxSamplerAnisotropy;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	VK_CHECK(vkCreateSampler(*VK_Device::Get().GetDevice(), &samplerInfo, VK_NULL_HANDLE, &m_sampler),
		"Failed to create sampler");
}

void VK_Texture::GenerateMipMaps(VkCommandBuffer _cmdBuff, Vec2u& _texSize)
{
	LOGGER_INFO("Generate Mipmap ! ");

	VkFormatProperties2 formatProps{};
	formatProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
	vkGetPhysicalDeviceFormatProperties2(*VK_Device::Get().GetPhycicalDevice(), m_format, &formatProps);

	if (!(formatProps.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		LOGGER_CRITICAL("Don't support mip map generation ! ");
	}

	Vec2u mipSize = _texSize;

	for (uint32_t i = 1; i < m_mipLevels; ++i)
	{
		LOGGER_INFO("First barrier before ! ");
		TransitionLayout(_cmdBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i - 1, 1);
		LOGGER_INFO("First barrier after ! ");

		VkImageSubresourceLayers srcSubresourceLayers{};
		srcSubresourceLayers.aspectMask = m_aspect;
		srcSubresourceLayers.mipLevel = i - 1;
		srcSubresourceLayers.layerCount = 1;

		VkImageSubresourceLayers dstSubresourceLayers{};
		dstSubresourceLayers.aspectMask = m_aspect;
		dstSubresourceLayers.mipLevel = i;
		dstSubresourceLayers.layerCount = 1;

		VkImageBlit2 blit{};
		blit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
		blit.pNext = VK_NULL_HANDLE;

		blit.srcSubresource = srcSubresourceLayers;
		blit.srcOffsets[0] = {};
		blit.srcOffsets[1] = { static_cast<int32_t>(mipSize.x), static_cast<int32_t>(mipSize.y), 1 };

		blit.dstSubresource = dstSubresourceLayers;
		blit.dstOffsets[0] = {};
		blit.dstOffsets[1] =
		{
			1 < mipSize.x ? static_cast<int32_t>(mipSize.x) / 2 : 1,
			1 < mipSize.y ? static_cast<int32_t>(mipSize.y) / 2 : 1,
			1
		};

		VkBlitImageInfo2 blitInfo{};
		blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
		blitInfo.pNext = VK_NULL_HANDLE;
		blitInfo.srcImage = m_image;
		blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		blitInfo.dstImage = m_image;
		blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitInfo.regionCount = 1;
		blitInfo.pRegions = &blit;
		blitInfo.filter = VK_FILTER_LINEAR;

		vkCmdBlitImage2(_cmdBuff, &blitInfo);

		LOGGER_INFO("Second barrier before ! ");
		TransitionLayout(_cmdBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i - 1, 1);
		LOGGER_INFO("Second barrier after ! ");

		if (1 < mipSize.x)
		{
			mipSize.x /= 2;
		}
		if (1 < mipSize.y)
		{
			mipSize.y /= 2;
		}
	}

	TransitionLayout(_cmdBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipLevels - 1, 1);

	LOGGER_INFO("Mipmap generated ! ");
}

VkFormat VK_Texture::GetFormat(ImageFormat _format, bool _sRGB)
{
	if (_sRGB)
	{
		switch (_format)
		{
		case IMAGE_FORMAT_R8G8B8A8:
			return VK_FORMAT_R8G8B8A8_SRGB;
			break;
		case IMAGE_FORMAT_R8G8B8:
			return VK_FORMAT_R8G8B8_SRGB;
			break;
		case IMAGE_FORMAT_R8G8:
			return VK_FORMAT_R8G8_SRGB;
			break;
		case IMAGE_FORMAT_R8:
			return VK_FORMAT_R8_SRGB;
			break;
		case IMAGE_FORMAT_MAX_ENUM:
			LOGGER_WARNING("Wrong image format !");
			return VK_FORMAT_R8G8B8A8_SRGB;
			break;
		}
	}
	else
	{
		switch (_format)
		{
		case IMAGE_FORMAT_R8G8B8A8:
			return VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case IMAGE_FORMAT_R8G8B8:
			return VK_FORMAT_R8G8B8_UNORM;
			break;
		case IMAGE_FORMAT_R8G8:
			return VK_FORMAT_R8G8_UNORM;
			break;
		case IMAGE_FORMAT_R8:
			return VK_FORMAT_R8_UNORM;
			break;
		case IMAGE_FORMAT_MAX_ENUM:
			LOGGER_WARNING("Wrong image format !");
			return VK_FORMAT_R8G8B8A8_UNORM;
			break;
		}
	}

	LOGGER_WARNING("Don't find the right format");
	return VK_FORMAT_R8G8B8A8_SRGB;
}

VkFilter VK_Texture::GetFilter(ImageFilter _filter)
{
	switch (_filter)
	{
	case FILTER_NEAREST:
		return VK_FILTER_NEAREST;
		break;
	case FILTER_LINEAR:
		return VK_FILTER_LINEAR;
		break;
	case FILTER_CUBIC:
		return VK_FILTER_CUBIC_EXT;
		break;
	case FILTER_MAX_ENUM:
		LOGGER_WARNING("Wrong filter ! ");
		return VK_FILTER_NEAREST;
		break;
	}

	LOGGER_WARNING("Don't find the right filter");
	return VK_FILTER_NEAREST;
}

VkSamplerMipmapMode VK_Texture::GetMipFilter(MipMapFilter _filter)
{
	switch (_filter)
	{
	case MIPMAP_FILTER_NEAREST:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case MIPMAP_FILTER_LINEAR:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case MIPMAP_FILTER_MAX_ENUM:
		LOGGER_WARNING("Wrong mip map filter ! ");
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	}

	LOGGER_WARNING("Don't find the right mip map filter ! ");
	return VK_SAMPLER_MIPMAP_MODE_NEAREST;
}

VkSamplerAddressMode VK_Texture::GetWrapMode(WrapMode _wrapMode)
{
	switch (_wrapMode) {
	case WRAP_MODE_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case WRAP_MODE_MIRRORED_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;
	case WRAP_MODE_CLAMP_TO_EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	case WRAP_MODE_MIRRORED_CLAMP_TO_EDGE:
		return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		break;
	case WRAP_MODE_CLAMP_TO_BORDER:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	case WRAP_MODE_MAX_ENUM:
		LOGGER_WARNING("Wrong wrap mode ! ");
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	}

	LOGGER_WARNING("Don't find the right wrap mode ! ");
	return VK_SAMPLER_ADDRESS_MODE_REPEAT;
}