#include "VK_VmaAllocatorWrapper.h"

#include "VK_Device.h"
#include "VK_Instance.h"

VK_VmaAllocatorWrapper& VK_VmaAllocatorWrapper::Get()
{
	static VK_VmaAllocatorWrapper instance;
	return instance;
}

void VK_VmaAllocatorWrapper::Init(VK_Device* _device, VK_Instance* _instance)
{
	LLYN_ASSERT(_device != nullptr || _instance != nullptr)

	m_device = _device;

	VmaVulkanFunctions vmaFunc{};
	vmaFunc.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vmaFunc.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	allocatorInfo.vulkanApiVersion = _instance->GetAPIVersion();
	allocatorInfo.physicalDevice = *m_device->GetPhycicalDevice();
	allocatorInfo.device = *m_device->GetDevice();
	allocatorInfo.instance = *_instance->GetInstance();
	allocatorInfo.pVulkanFunctions = &vmaFunc;

	VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator),
		"Failed to create vma allocator");
}

void VK_VmaAllocatorWrapper::Cleanup()
{
	vmaDestroyAllocator(m_allocator);
}

void VK_VmaAllocatorWrapper::CreateBuffer(const VkBufferCreateInfo& _buffInfo, VmaMemoryUsage _memUsage,
                                          VmaAllocationCreateFlags _flags, VkBuffer& _outBuff, VmaAllocation& _outAlloc, VmaAllocationInfo& _outAllocInfo) const
{
	VmaAllocationCreateInfo allocationCreateInfo{};
	allocationCreateInfo.usage = _memUsage;
	allocationCreateInfo.flags = _flags;

	VK_CHECK(vmaCreateBuffer(m_allocator, &_buffInfo, &allocationCreateInfo, &_outBuff, &_outAlloc, &_outAllocInfo),
		"Failed to create buffer ");
}

void VK_VmaAllocatorWrapper::DestroyBuffer(VkBuffer _buff, VmaAllocation _alloc) const
{
	vmaDestroyBuffer(m_allocator, _buff, _alloc);
}

void VK_VmaAllocatorWrapper::CreateImage(const VkImageCreateInfo& _imageInfo, VmaMemoryUsage _memUsage,
	VkImage& _outImage, VmaAllocation& _outAlloc) const
{
	VmaAllocationCreateInfo allocationCreateInfo{};
	allocationCreateInfo.usage = _memUsage;

	VK_CHECK(vmaCreateImage(m_allocator, &_imageInfo, &allocationCreateInfo, &_outImage, &_outAlloc, nullptr),
		"Failed to create image ");
}

void VK_VmaAllocatorWrapper::DestroyImage(VkImage _image, VmaAllocation _alloc) const
{
	vmaDestroyImage(m_allocator, _image, _alloc);
}

void VK_VmaAllocatorWrapper::MapMemory(void** _mem, VmaAllocation _allocation)
{
	VK_CHECK(vmaMapMemory(m_allocator, _allocation, _mem),
		"Failed to map memory ! ");
}

void VK_VmaAllocatorWrapper::UnMapMemory(VmaAllocation _allocation)
{
	vmaUnmapMemory(m_allocator, _allocation);
}