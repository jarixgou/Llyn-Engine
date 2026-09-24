#ifndef VK_DEPTH_RESOURCES__H
#define VK_DEPTH_RESOURCES__H

#include <vulkan/vulkan.h>

#include "../../LlynCore.h"

class VK_DepthResources
{
private:
	VkImage m_image = VK_NULL_HANDLE;
	VkImageView m_imageView = VK_NULL_HANDLE;
	VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
public:
	void Init(const VK_Device* _device, VkExtent2D _extent);

	void Cleanup(const VK_Device* _device) const;

	const VkImage* GetImage();
	const VkImageView* GetImageView();

	static VkFormat FinDepthFormat(const VK_Device* _device);
private:
};

#endif