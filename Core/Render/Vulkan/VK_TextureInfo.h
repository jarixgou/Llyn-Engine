#ifndef VK_TEXTURE_INFO__H
#define VK_TEXTURE_INFO__H

#include <vulkan/vulkan_core.h>

#include "../../Asset/TextureConfig.h"
#include "../../Vector/Vec2.h"

struct VK_TextureInfo
{
	TextureConfig config;
	VkFormat preferredFormat = VK_FORMAT_UNDEFINED;
	VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageAspectFlags aspect = VK_IMAGE_ASPECT_NONE;
	Vec2u textureSize = {0, 0};
	void* data = nullptr;
};

#endif