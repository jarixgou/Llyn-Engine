#include "VK_MemoryHelper.h"

#include "../../Render/Vulkan/VK_Device.h"

uint32_t VK_MemoryHelper::FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties)
{
	VkPhysicalDeviceMemoryProperties memProperties{};
	vkGetPhysicalDeviceMemoryProperties(*VK_Device::Get().GetPhycicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (_typeFilter & (1 << i) &&
			(memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
		{
			return i;
		}
	}

	// TODO: print error with logger
	return 0;
}

VkDeviceMemory VK_MemoryHelper::AllocateImageDeviceMemory(VkImage* _image, VkMemoryPropertyFlags _properties)
{
	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(*VK_Device::Get().GetDevice(), *_image, &memRequirements);

	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext = VK_NULL_HANDLE;
	memAllocInfo.allocationSize = memRequirements.size;
	memAllocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

	VkDeviceMemory deviceMem = VK_NULL_HANDLE;
	VK_CHECK(vkAllocateMemory(*VK_Device::Get().GetDevice(), &memAllocInfo, VK_NULL_HANDLE, &deviceMem)
	," Failed to allocate memory !");

	VK_CHECK(vkBindImageMemory(*VK_Device::Get().GetDevice(), *_image, deviceMem, 0),
		"Failed to bind image memory !");

	return deviceMem;
}

VkDeviceMemory VK_MemoryHelper::AllocateDeviceMemory(VkBuffer _buffer, VkMemoryPropertyFlags _propertyFlags)
{
	VkMemoryRequirements memRequirements{};
	vkGetBufferMemoryRequirements(*VK_Device::Get().GetDevice(), _buffer, &memRequirements);

	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext = VK_NULL_HANDLE;
	memAllocInfo.allocationSize = memRequirements.size;
	memAllocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _propertyFlags);

	VkDeviceMemory deviceMem = VK_NULL_HANDLE;

	VK_CHECK(vkAllocateMemory(*VK_Device::Get().GetDevice(), &memAllocInfo, nullptr, &deviceMem), "Failed to Allocate Memory !");

	VK_CHECK(vkBindBufferMemory(*VK_Device::Get().GetDevice(), _buffer, deviceMem, 0), "Failed to Bind Buffer Memory");

	return deviceMem;
}
