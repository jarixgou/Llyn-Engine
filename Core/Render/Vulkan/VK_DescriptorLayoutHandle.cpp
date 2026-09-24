#include "VK_DescriptorLayoutHandle.h"

#include <iostream>

#include "VK_Device.h"
#include "VK_LayoutInfo.h"
#include "../../Logger.h"

void VK_DescriptorLayoutHandle::CreateLayout(const VK_Device* _device, std::vector<VK_LayoutInfo>& _infos)
{
    LLYN_ASSERT(_device != nullptr);

    if (_infos.empty())
    {
        LOGGER_ERROR("Can't create the descriptor layout because VK_LayoutInfo is empty");
        return;
    }

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings(_infos.size());
    std::vector<VkDescriptorBindingFlags> bindingFlags;

    bool bindless = false;
    for (int i = 0; i < _infos.size(); ++i)
    {
        layoutBindings[i].binding = _infos[i].binding;
        layoutBindings[i].descriptorType = _infos[i].type;
        layoutBindings[i].descriptorCount = _infos[i].count;
        layoutBindings[i].stageFlags = _infos[i].stageFlags;
        layoutBindings[i].pImmutableSamplers = VK_NULL_HANDLE;

        if (_infos[i].bindless)
        {
            bindingFlags.emplace_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | 
            	VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            bindless = true;
        }
        else
        {
            bindingFlags.emplace_back(0);
        }
    }

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
    bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlagsCreateInfo.pNext = VK_NULL_HANDLE;
    bindingFlagsCreateInfo.bindingCount = bindless ? static_cast<uint32_t>(bindingFlags.size()) : 0;
    bindingFlagsCreateInfo.pBindingFlags = bindless ? bindingFlags.data() : VK_NULL_HANDLE;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext = &bindingFlagsCreateInfo;
    layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutCreateInfo.pBindings = layoutBindings.data();
    layoutCreateInfo.flags = bindless ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT : 0;

    infos = _infos;
    VK_CHECK(vkCreateDescriptorSetLayout(*_device->GetDevice(), &layoutCreateInfo, VK_NULL_HANDLE, &layout),
        "Failed to create descriptor set layout");
}

bool VK_DescriptorLayoutHandle::operator==(const VK_DescriptorLayoutHandle& _b) const
{
    if (infos.size() != _b.infos.size())
    {
        return false;
    }

    for (size_t i = 0; i < infos.size(); ++i)
    {
        const VK_LayoutInfo& ia = infos[i];
        const VK_LayoutInfo& ib = _b.infos[i];
        if (ia.name != ib.name || ia.binding != ib.binding || ia.offset != ib.offset || 
        	ia.count != ib.count || ia.type != ib.type)
            return false;
    }
    return true;
}
