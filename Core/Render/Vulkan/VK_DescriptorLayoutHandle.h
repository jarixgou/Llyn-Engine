#ifndef VK_DESCRIPTOR_LAYOUT_HANDLE__H
#define VK_DESCRIPTOR_LAYOUT_HANDLE__H

#include <vector>
#include <vulkan/vulkan.h>

#include "../../LlynCore.h"
#include "VK_LayoutInfo.h"

struct VK_DescriptorLayoutHandle
{
	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	std::vector<VK_LayoutInfo> infos;

    void CreateLayout(const VK_Device* _device, std::vector<VK_LayoutInfo>& _infos);

    bool operator==(const VK_DescriptorLayoutHandle& _b) const;
};

#endif
