#include "VK_Texture.h"

#include "VK_Buffer.h"
#include "VK_TextureInfo.h"
#include "VK_VmaAllocatorWrapper.h"

void VK_Texture::Init(VK_TextureInfo& _info)
{
	const uint32_t mipLevels = static_cast<uint32_t>(floor(log2(std::max(_info.textureSize.x, _info.textureSize.y))) + 1);
	const uint32_t actualMipLevels = _info.generateMipMaps ? mipLevels : 1;

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = VK_NULL_HANDLE;
	imageInfo.format = _info.format;
	imageInfo.extent = { _info.textureSize.x, _info.textureSize.y, 1 };
	imageInfo.mipLevels = actualMipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = _info.usage;

	VK_Image::Init(_info.format, _info.layout, _info.type, actualMipLevels);

	VK_VmaAllocatorWrapper::Get().CreateImage(imageInfo, VMA_MEMORY_USAGE_AUTO, m_image, m_allocation);

	const VkDeviceSize imageSizeByte = _info.textureSize.x * _info.textureSize.y * 4;
	VK_Buffer stagingBuff = VK_Buffer::CreateStagingBuffer(_info.data, imageSizeByte, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	VkCommandBuffer cmdBuff = VK_Buffer::BeginSingleTimeCommand();

	TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VK_Buffer::CopyBufferToImage(cmdBuff, &stagingBuff, &m_image, _info.textureSize);

	if (_info.generateMipMaps)
	{
		// TODO: Create mip map generation
	}
	else
	{
		TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VK_Buffer::EndSingleTimeCommand(cmdBuff);


}
