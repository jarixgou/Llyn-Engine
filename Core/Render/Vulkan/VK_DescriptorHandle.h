#ifndef VK_DESCRIPTOR_HANDLE__H
#define VK_DESCRIPTOR_HANDLE__H

#include <vector>
#include <vulkan/vulkan.h>

#include "VK_ShaderBuffer.h"

struct VK_DescriptorHandle
{
	VkDescriptorPool descriptorPools = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> descriptorSet;
	std::vector<VK_ShaderBuffer> shaderBuffer;
};

#endif