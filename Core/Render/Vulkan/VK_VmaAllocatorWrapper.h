#ifndef VK_VMA_ALLOCATOR_WRAPPER__H
#define VK_VMA_ALLOCATOR_WRAPPER__H

#include <VMA/vk_mem_alloc.h>

#include "../../LlynCore.h"

class VK_VmaAllocatorWrapper
{
private:
	VmaAllocator m_allocator = nullptr;
public:
	static VK_VmaAllocatorWrapper& Get();

	void Init(VK_Instance* _instance);
	void Cleanup();

	void CreateBuffer(const VkBufferCreateInfo& _buffInfo, VmaMemoryUsage _memUsage,
	                  VmaAllocationCreateFlags _flags, VkBuffer& _outBuff, VmaAllocation& _outAlloc, VmaAllocationInfo& _outAllocInfo) const;
	void DestroyBuffer(VkBuffer _buff, VmaAllocation _alloc) const;

	void CreateImage(const VkImageCreateInfo& _imageInfo, VmaMemoryUsage _memUsage,
	                 VmaAllocationCreateFlags _flags, VkImage& _outImage, VmaAllocation& _outAlloc) const;
	void DestroyImage(VkImage _image, VmaAllocation _alloc) const;

	void MapMemory(void** _mem, VmaAllocation _allocation);
	void UnMapMemory(VmaAllocation _allocation);
};

#endif