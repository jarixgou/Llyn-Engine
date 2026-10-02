#ifndef VK_VMA_ALLOCATOR_WRAPPER__H
#define VK_VMA_ALLOCATOR_WRAPPER__H

#include <VMA/vk_mem_alloc.h>

#include "../../LlynCore.h"

class VK_VmaAllocatorWrapper
{
private:
	VmaAllocator m_allocator = nullptr;
	VK_Device* m_device = nullptr;
public:
	void Init(VK_Device* _device, VK_Instance* _instance);
};

#endif