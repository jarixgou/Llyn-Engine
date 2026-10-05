#include "VK_Texture.h"

#include "VK_Buffer.h"
#include "VK_VmaAllocatorWrapper.h"
#include "../../Vector/Vec2.h"

void VK_Texture::Init(VkImageUsageFlags _usage, VkFormat _format, VkImageLayout _layout, VkImageType _type, Vec2<uint32_t>& _texSize, void* data)
{
	uint32_t mipLevels = static_cast<uint32_t>(floor(log2(std::max(_texSize.x, _texSize.y))) + 1);

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = VK_NULL_HANDLE;
	imageInfo.format = _format;
	imageInfo.extent = { _texSize.x, _texSize.y, 1 };
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = _usage;

	VK_Image::Init(_format, _layout, _type, mipLevels);

	VK_VmaAllocatorWrapper::Get().CreateImage(imageInfo, VMA_MEMORY_USAGE_AUTO, m_image, m_allocation);

	VK_Buffer stagingBuff = VK_Buffer::CreateStagingBuffer();

	VkCommandBuffer cmdBuff = VK_Buffer::BeginSingleTimeCommand();

	TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}
