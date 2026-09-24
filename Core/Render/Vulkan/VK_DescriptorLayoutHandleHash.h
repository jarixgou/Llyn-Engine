#ifndef VK_DESCRIPTOR_LAYOUT_HANDLE_HASH__H
#define VK_DESCRIPTOR_LAYOUT_HANDLE_HASH__H

#include "../../LlynCore.h"

struct VK_DescriptorLayoutHandleHash
{
	size_t operator()(const VK_DescriptorLayoutHandle& _layout) const;
};

#endif