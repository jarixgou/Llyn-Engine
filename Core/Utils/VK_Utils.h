#ifndef VULKAN_UTILS__H
#define VULKAN_UTILS__H

#include <vector>
#include <vulkan/vulkan.h>

#include "../Render/Vulkan/VK_Device.h"
#include "../Vector/FwdVec2.h"

class VK_Device;

namespace Utils
{
	VkFormat FindSupportedFormat(const VK_Device* _device, const std::vector<VkFormat>& _candidates,
	                             VkImageTiling _tiling, VkFormatFeatureFlags _features);

	std::pair<VkImage, VkDeviceMemory> CreateImage(const VK_Device* _device,
	                                               const Vec2<uint32_t>& _imageSize, VkFormat _format, VkImageTiling _tiling,
	                                               VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
	VkImageView CreateImageView(const VK_Device* _device, VkImage const& _image,
	                            VkFormat _format, VkImageAspectFlags _aspectFlags);

	uint32_t FindMemoryType(const VK_Device* _device, uint32_t _typeFilter, VkMemoryPropertyFlags _properties);

	VkDescriptorSetLayout CreateLayout(const VK_Device* _device, const std::vector<uint32_t>& _bindings, const std::vector<uint32_t>& _counts,
	                                   const std::vector<VkDescriptorType>& _types, const std::vector<VkShaderStageFlags>& _stages);

	VkDescriptorSetLayout CreateBindlessLayout(const VK_Device* _device, const std::vector<uint32_t>& _bindings, const std::vector<uint32_t>& _counts, const std::
	                                           vector<VkDescriptorType>& _types, const std::vector<VkShaderStageFlags>& _stages);
}

#endif