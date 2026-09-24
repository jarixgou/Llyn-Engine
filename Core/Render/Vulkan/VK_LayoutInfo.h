#ifndef VK_LAYOUT_INFO__H
#define VK_LAYOUT_INFO__H

#include <string>
#include <vulkan/vulkan_core.h>

struct VK_LayoutInfo
{
	std::string name;
	uint32_t binding = 0;
	uint32_t offset = 0;
	uint32_t count = 0;
	VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	VkShaderStageFlags stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bool bindless = false;
};

#endif