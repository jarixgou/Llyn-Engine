#ifndef VK_IMAGE__H
#define VK_IMAGE__H
#include <vulkan/vulkan_core.h>

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
private:
	uint32_t m_mipsLevel = 0;
	bool m_swapChain = false;
public:
	void Init(VkFormat _format, VkImageLayout _layout, VkImageType _type, uint32_t _mipsLevel, bool _swapChain = false);
	void Cleanup(VK_Device* _device);

	void TransitionLayout(VkCommandBuffer _cmdBuff, VkImageLayout _newLayout);
	void TransitionLayout(VkCommandBuffer _cmdBuff, VkImageLayout _oldLayout, VkImageLayout _newLayout);

	bool HasStencilComponent(VkFormat _format);
private:
	void ImageBarrier(VkCommandBuffer _cmdBuff, VkImageLayout _oldLayout,
	                  VkImageLayout _newLayout, uint32_t _layerCount, uint32_t _baseMipsLevel);

	VkImageAspectFlags GetImageAspect(VkImageLayout _layout);
	VK_StageAndAccess GetStageAndAccess(VkImageLayout _oldLayout, VkImageLayout _newLayout);
};

#endif