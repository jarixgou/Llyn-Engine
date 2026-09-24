#ifndef VK_DEVICE__H
#define VK_DEVICE__H

#include <vector>
#include <vulkan/vulkan.h>

#include "../../LlynCore.h"

class VK_Device
{
private:
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;

	uint32_t m_queueIndex = ~0;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;

	static std::vector<const char*> m_requiredDeviceExtension;
public:
	void Init(VK_Instance* _instance);
	void Cleanup();

	const VkPhysicalDevice* GetPhycicalDevice() const;
	const VkDevice* GetDevice() const;

	const VkQueue* GetGraphicsQueue() const;

	uint32_t GetQueueIndex() const;
private:
	void PickPhysicalDevice(const VkInstance* _instance);
	void CreateLogicalDevice(const VkSurfaceKHR* _surface);
	static bool IsDeviceSuitable(const VkPhysicalDevice& _physicalDevice);
};

#endif