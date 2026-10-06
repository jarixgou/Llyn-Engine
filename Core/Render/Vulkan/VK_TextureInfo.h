#ifndef VK_TEXTURE_INFO__H
#define VK_TEXTURE_INFO__H

#include <vulkan/vulkan_core.h>

#include "../../Vector/Vec2.h"

struct VK_TextureInfo
{
	VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageType type = VK_IMAGE_TYPE_2D;
	Vec2u textureSize = {0, 0};
	void* data = nullptr;
	bool generateMipMaps = false;
	bool createSampler = false;
};

#endif