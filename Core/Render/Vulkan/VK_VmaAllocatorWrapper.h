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
	static VK_VmaAllocatorWrapper& Get();

	void Init(VK_Device* _device, VK_Instance* _instance);
	void Cleanup();

	void CreateBuffer(const VkBufferCreateInfo& _buffInfo, VmaMemoryUsage _memUsage,
		VkBuffer& _outBuff, VmaAllocation& _outAlloc) const;
	void DestroyBuffer(VkBuffer _buff, VmaAllocation _alloc) const;

	void CreateImage(const VkImageCreateInfo& _imageInfo, VmaMemoryUsage _memUsage,
		VkImage& _outImage, VmaAllocation& _outAlloc) const;
	void DestroyImage(VkImage _image, VmaAllocation _alloc) const;
};

#endif