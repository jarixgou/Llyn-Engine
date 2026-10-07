#ifndef VK_IMAGE__H
#define VK_IMAGE__H
#include <vulkan/vulkan_core.h>

#include "VK_VmaAllocatorWrapper.h"
#include "../../Vector/FwdVec2.h"
#include "../../LlynCore.h"

class VK_Image
{
public:
	VkImage m_image = VK_NULL_HANDLE;
	VkImageView m_imageView = VK_NULL_HANDLE;
protected:
	VkFormat m_format = VK_FORMAT_UNDEFINED;
	VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageType m_type = VK_IMAGE_TYPE_2D;
	VkImageViewType m_viewType = VK_IMAGE_VIEW_TYPE_2D;
	VkImageAspectFlags m_aspect = VK_IMAGE_ASPECT_NONE;
	VkImageUsageFlags m_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	uint32_t m_mipLevels = 0;
private:
	VmaAllocation m_allocation = VK_NULL_HANDLE;
	bool m_swapChain = false;
public:

	void Init(VkFormat _format, VkImageLayout _layout, const ImageType& _type, VkImageAspectFlags _aspect,
	          VkImageUsageFlags _usage, uint32_t _mipsLevel, const Vec2u& _textureSize, bool _swapChain = false);
	void Cleanup();

	void CreateView();

	void TransitionLayout(VkCommandBuffer _cmdBuff, VkImageLayout _newLayout, uint32_t _baseMipLevel);
	void TransitionLayout(VkCommandBuffer _cmdBuff, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _baseMipLevel);

	bool HasStencilComponent(VkFormat _format);
private:
	static VkImageType GetType(ImageType _type);
	static VkImageViewType GetViewType(ImageType _type);

	void ImageBarrier(VkCommandBuffer _cmdBuff, VkImageLayout _oldLayout,
	                  VkImageLayout _newLayout, uint32_t _layerCount, uint32_t _baseMipsLevel);

	VkImageAspectFlags GetImageAspect(VkImageLayout _layout);
	VK_StageAndAccess GetStageAndAccess(VkImageLayout _oldLayout, VkImageLayout _newLayout);
};

#endif