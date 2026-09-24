#include "Texture.h"

#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>
#include <JSON/simdjson.h>
#include <JSON/json.hpp>

#include "../Render/Vulkan/VK_Device.h"
#include "../Render/Vulkan/VK_Buffer.h"
#include "../Utils/Vulkan/VK_MemoryHelper.h"
#include "../Vector/Vec2.h"

VK_Device* Texture::m_device = nullptr;

Texture::~Texture()
{
	
}

bool Texture::Load(const std::string& _filePath)
{
	int texWidth = 0;
	int texHeight = 0;
	int texChannels = 0;

	stbi_uc* pixels = stbi_load(_filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (pixels == nullptr)
	{
		return false;
	}

	std::string metaDataPath = _filePath + ".metadata";

	// Load texture config
	LoadConfig(_filePath);


	m_mipLevels = static_cast<uint32_t>(floor(log2(std::max(texWidth, texHeight))) + 1);

	std::tie(m_image, m_memory) = CreateImage(Vec2<uint32_t>(texWidth, texHeight),
	                                          m_mipLevels,
	                                          m_config.imageType,
	                                          VK_FORMAT_R8G8B8A8_SRGB,
	                                          VK_IMAGE_TILING_OPTIMAL,
	                                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
	                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	VK_Buffer stagingBuffer{};
	stagingBuffer = VK_Buffer::CreateStagingBuffer(pixels, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	stbi_image_free(pixels);
	
	VkCommandBuffer cmdBuffer = VK_Buffer::BeginSingleTimeCommand();

	TransitionImageLayout(cmdBuffer, &m_image, m_mipLevels,
	                      VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	                      VK_ACCESS_2_NONE, VK_ACCESS_2_TRANSFER_WRITE_BIT,
	                      VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_Buffer::CopyBufferToImage(cmdBuffer, &stagingBuffer, &m_image, Vec2<uint32_t>(texWidth, texHeight));

	GenerateMipMaps(m_device, cmdBuffer, &m_image, VK_FORMAT_R8G8B8A8_SRGB, Vec2<uint32_t>(texWidth, texHeight), m_mipLevels);

	VK_Buffer::EndSingleTimeCommand(cmdBuffer);

	stagingBuffer.Cleanup();

	VkFormat format = m_config.sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

	m_imageView = CreateImageView(m_mipLevels, m_config.imageViewType, &m_image, format, VK_IMAGE_ASPECT_COLOR_BIT);
	m_sampler = CreateSampler(m_config.samplerFilter, m_config.samplerMimapMode, m_config.wrapMode);

	return true;
}

void Texture::Unload()
{
	vkFreeMemory(*m_device->GetDevice(), m_memory, VK_NULL_HANDLE);
	vkDestroyImage(*m_device->GetDevice(), m_image, VK_NULL_HANDLE);
	vkDestroyImageView(*m_device->GetDevice(), m_imageView, VK_NULL_HANDLE);
	vkDestroySampler(*m_device->GetDevice(), m_sampler, VK_NULL_HANDLE);
}

bool Texture::Add(const std::string& _filePath, IRessource* _ressource)
{

	return false;
}

VkImageView* Texture::GetImageView()
{
	return &m_imageView;
}

VkSampler* Texture::GetSampler()
{
	return &m_sampler;
}

void Texture::SetDevice(VK_Device* _device)
{
	m_device = _device;
}

std::pair<VkImage, VkDeviceMemory> Texture::CreateImage(Vec2<uint32_t> _imageSize, uint32_t _mipLevels,
                                                        VkImageType _imageType, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = VK_NULL_HANDLE;
	imageInfo.imageType = _imageType;
	imageInfo.format = _format;
	imageInfo.extent = { _imageSize.x, _imageSize.y, 1 };
	imageInfo.mipLevels = _mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = _tiling;
	imageInfo.usage = _usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkImage image = VK_NULL_HANDLE;
	vkCreateImage(*m_device->GetDevice(), &imageInfo, VK_NULL_HANDLE, &image);

	VkDeviceMemory mem = VK_NULL_HANDLE;
	mem = VK_MemoryHelper::AllocateImageDeviceMemory(&image, _properties);

	return { image, mem };
}

VkImageView Texture::CreateImageView(uint32_t _mipLevels, VkImageViewType _imageType, VkImage* _image, VkFormat _format, VkImageAspectFlags _aspect)
{
	VkImageSubresourceRange imgSubresourceRange{};
	imgSubresourceRange.aspectMask = _aspect;
	imgSubresourceRange.baseMipLevel = 0;
	imgSubresourceRange.levelCount = _mipLevels;
	imgSubresourceRange.baseArrayLayer = 0;
	imgSubresourceRange.layerCount = 1;

	VkImageViewCreateInfo imgViewInfo{};
	imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imgViewInfo.pNext = VK_NULL_HANDLE;
	imgViewInfo.image = *_image;
	imgViewInfo.viewType = _imageType;
	imgViewInfo.format = _format;
	imgViewInfo.subresourceRange = imgSubresourceRange;

	VkImageView imgView = VK_NULL_HANDLE;
	VK_CHECK(vkCreateImageView(*m_device->GetDevice(), &imgViewInfo, VK_NULL_HANDLE, &imgView),
		"Failed to create image view !");

	return imgView;
}

void Texture::GenerateMipMaps(const VK_Device* _device, VkCommandBuffer _cmdBuffer,
                              VkImage* _image, VkFormat _format, Vec2<uint32_t> _imageSize, uint32_t _mipLevels)
{
	LLYN_ASSERT(_device != nullptr);
	VkFormatProperties2 formatProps{};
	formatProps.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
	vkGetPhysicalDeviceFormatProperties2(*_device->GetPhycicalDevice(), _format, &formatProps);

	if (!(formatProps.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		// Print error with logger !
	}

	VkImageSubresourceRange subResource{};
	subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subResource.baseMipLevel = 0;
	subResource.levelCount = 1;
	subResource.baseArrayLayer = 0;
	subResource.layerCount = 1;

	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = VK_NULL_HANDLE;
	barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = *_image;
	barrier.subresourceRange = subResource;

	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.pNext = VK_NULL_HANDLE;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	Vec2<uint32_t> mipSize = _imageSize;

	for (uint32_t i = 1; i < _mipLevels; ++i)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;

		barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;

		vkCmdPipelineBarrier2(_cmdBuffer, &dependencyInfo);

		VkImageSubresourceLayers srcSubresourcesLayers{};
		srcSubresourcesLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		srcSubresourcesLayers.mipLevel = i - 1;
		srcSubresourcesLayers.layerCount = 1;

		VkImageSubresourceLayers dstSubresourcesLayers{};
		dstSubresourcesLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		dstSubresourcesLayers.mipLevel = i;
		dstSubresourcesLayers.layerCount = 1;

		VkImageBlit2 imageBlit{};
		imageBlit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
		imageBlit.pNext = VK_NULL_HANDLE;

		imageBlit.srcSubresource = srcSubresourcesLayers;
		imageBlit.srcOffsets[0] = {};
		imageBlit.srcOffsets[1] = { static_cast<int32_t>(mipSize.x), static_cast<int32_t>(mipSize.y), 1 };

		imageBlit.dstSubresource = dstSubresourcesLayers;
		imageBlit.dstOffsets[0] = {};
		imageBlit.dstOffsets[1] = {
			1 < mipSize.x ? static_cast<int32_t>(mipSize.x) / 2 : 1,  
			1 < mipSize.y ? static_cast<int32_t>(mipSize.y) / 2 : 1, 
			1
		};

		VkBlitImageInfo2 blitImgInfo{};
		blitImgInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
		blitImgInfo.pNext = VK_NULL_HANDLE;
		blitImgInfo.srcImage = *_image;
		blitImgInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		blitImgInfo.dstImage = *_image;
		blitImgInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitImgInfo.regionCount = 1;
		blitImgInfo.pRegions = &imageBlit;
		blitImgInfo.filter = VK_FILTER_LINEAR;

		vkCmdBlitImage2(_cmdBuffer, &blitImgInfo);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

		barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

		vkCmdPipelineBarrier2(_cmdBuffer, &dependencyInfo);

		if (1 < mipSize.x)
		{
			mipSize.x /= 2;
		}
		if (1 < mipSize.y)
		{
			mipSize.y /= 2;
		}
	}

	barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

	barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

	vkCmdPipelineBarrier2(_cmdBuffer, &dependencyInfo);
}

VkSampler Texture::CreateSampler(VkFilter _filter, VkSamplerMipmapMode _samplerMipmap, VkSamplerAddressMode _samplerAddressMode)
{
	VkPhysicalDeviceProperties physicDeviceProps{};
	vkGetPhysicalDeviceProperties(*m_device->GetPhycicalDevice(), &physicDeviceProps);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = VK_NULL_HANDLE;
	samplerInfo.magFilter = _filter;
	samplerInfo.minFilter = _filter;
	samplerInfo.mipmapMode = _samplerMipmap;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
	samplerInfo.addressModeU = _samplerAddressMode;
	samplerInfo.addressModeV = _samplerAddressMode;
	samplerInfo.addressModeW = _samplerAddressMode;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = physicDeviceProps.limits.maxSamplerAnisotropy;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	VkSampler sampler = VK_NULL_HANDLE;
	VK_CHECK(vkCreateSampler(*m_device->GetDevice(), &samplerInfo, VK_NULL_HANDLE, &sampler),
		"Failed to create sampler !");

	return sampler;
}

void Texture::TransitionImageLayout(VkCommandBuffer _cmdBuffer, const VkImage* _image, uint32_t _mipLevels,
                                    VkImageLayout _oldLayout, VkImageLayout _newLayout, VkAccessFlags2 _srcAccessMask,
                                    VkAccessFlags2 _dstAccessMask, VkPipelineStageFlags2 _srcStageMask, VkPipelineStageFlags2 _dstStageMask, VkImageAspectFlags _imageAspectFlags)
{
	LLYN_ASSERT(_image != nullptr);

	VkImageSubresourceRange subresourceRange{};
	subresourceRange.aspectMask = _imageAspectFlags;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = _mipLevels;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = VK_NULL_HANDLE;
	barrier.srcStageMask = _srcStageMask;
	barrier.srcAccessMask = _srcAccessMask;
	barrier.dstStageMask = _dstStageMask;
	barrier.dstAccessMask = _dstAccessMask;
	barrier.oldLayout = _oldLayout;
	barrier.newLayout = _newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = *_image;
	barrier.subresourceRange = subresourceRange;

	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.pNext = VK_NULL_HANDLE;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(_cmdBuffer, &dependencyInfo);
}

void Texture::LoadConfig(const std::string& _filePath)
{
	std::string metaDataPath = _filePath + ".metadata";


	if (std::filesystem::exists(metaDataPath))
	{
		simdjson::padded_string json;
		if (simdjson::padded_string::load(metaDataPath).get(json))
		{
			simdjson::ondemand::parser parser;
			simdjson::ondemand::document data;

			if (parser.iterate(json).get(data))
			{
				// print error with logger
			}

			simdjson::ondemand::object root = data.get_object();
			for (auto field : root)
			{
				std::string_view key = field.unescaped_key();
				if (key == "ImageType" && field.value().is_integer())
				{
					m_config.imageType = static_cast<VkImageType>(field.value().get_int32().value());
					m_config.imageViewType = static_cast<VkImageViewType>(field.value().get_int32().value());
				}
				else if (key == "sRGB" && field.value().is_integer())
				{
					m_config.sRGB = field.value().get_bool();
				}
				else if (key == "WrapMode" && field.value().is_integer())
				{
					m_config.wrapMode = static_cast<VkSamplerAddressMode>(field.value().get_int32().value());
				}
				else if (key == "FilterMode" && field.value().is_integer())
				{
					m_config.samplerFilter = static_cast<VkFilter>(field.value().get_int32().value());
				}
				else if (key == "Mipmap" && field.value().is_integer())
				{
					m_config.mipmap = field.value().get_bool();
				}
				else if (key == "MipmapFilter" && field.value().is_integer())
				{
					m_config.samplerMimapMode = static_cast<VkSamplerMipmapMode>(field.value().get_int32().value());
				}
			}
		}
	}
	else
	{
		m_config = TextureConfig{};
		nlohmann::json json;

		json["ImageType"] = m_config.imageType;
		json["sRGB"] = m_config.sRGB;
		json["WrapMode"] = m_config.wrapMode;
		json["FilterMode"] = m_config.samplerFilter;
		json["Mipmap"] = m_config.mipmap;
		json["MipmapFilter"] = m_config.samplerMimapMode;

		std::ofstream file(metaDataPath);
		file << json.dump(4);
		file.close();
	}
}
