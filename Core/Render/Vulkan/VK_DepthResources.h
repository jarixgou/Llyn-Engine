#ifndef VK_DEPTH_RESOURCES__H
#define VK_DEPTH_RESOURCES__H

#include <vulkan/vulkan.h>

#include "../../LlynCore.h"
#include "VK_Image.h"

class VK_DepthResources
{
private:
	VK_Image m_image;
public:
	void Init(VkExtent2D _extent);

	void Cleanup();

	VK_Image* GetImage();

	static VkFormat FinDepthFormat(const VK_Device* _device);
private:
};

#endif