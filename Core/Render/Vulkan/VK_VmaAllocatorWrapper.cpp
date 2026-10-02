#include "VK_VmaAllocatorWrapper.h"

void VK_VmaAllocatorWrapper::Init(VK_Device* _device, VK_Instance* _instance)
{
	LLYN_ASSERT(_device != nullptr || _instance != nullptr)

	m_device = _device;

	VmaVulkanFunctions vmaFunc{};
	vmaFunc.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vmaFunc.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorInfo[];
}