#ifndef VK_MEMORY_HELPER__H
#define VK_MEMORY_HELPER__H

#include <vulkan/vulkan.h>

class VK_Device;

class VK_MemoryHelper
{
private:
	static VK_Device* m_device;
public:
	static void SetDevice(VK_Device* _device);

	static uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties);

	static VkDeviceMemory AllocateImageDeviceMemory(VkImage* _image, VkMemoryPropertyFlags _properties);
	static VkDeviceMemory AllocateDeviceMemory(VkBuffer _buffer, VkMemoryPropertyFlags _propertyFlag);
};

#endif