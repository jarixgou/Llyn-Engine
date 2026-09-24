#include "VK_Device.h"

#include "../../LlynCore.h"

#include "VK_Instance.h"
#include "../../Logger.h"

std::vector<const char*> VK_Device::m_requiredDeviceExtension = 
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

void VK_Device::Init(VK_Instance* _instance)
{
	if (_instance == nullptr)
	{
		// TODO: print error message with the logger
		return;
	}

	PickPhysicalDevice(_instance->GetInstance());
	CreateLogicalDevice(_instance->GetSurface());
}

void VK_Device::Cleanup()
{
	LOGGER_INFO("Destroying VK_Device");

	vkDestroyDevice(m_device, VK_NULL_HANDLE);
	LOGGER_INFO("VkDevice is destroyed");
}

const VkPhysicalDevice* VK_Device::GetPhycicalDevice() const
{
	return &m_physicalDevice;
}

const VkDevice* VK_Device::GetDevice() const
{
	return &m_device;
}

const VkQueue* VK_Device::GetGraphicsQueue() const
{
	return &m_graphicsQueue;
}

uint32_t VK_Device::GetQueueIndex() const
{
	return m_queueIndex;
}

void VK_Device::PickPhysicalDevice(const VkInstance* _instance)
{
	LLYN_ASSERT(_instance != nullptr);

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		// TODO : print error message with logger
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*_instance, &deviceCount, devices.data());

	for (size_t i = 0; i < devices.size(); ++i)
	{
		if (IsDeviceSuitable(devices[i]))
		{
			m_physicalDevice = devices[i];
			i = devices.size();
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		// TODO : print error message with logger 
	}
}

void VK_Device::CreateLogicalDevice(const VkSurfaceKHR* _surface)
{
	LLYN_ASSERT(_surface != nullptr);

	uint32_t queuePropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queuePropertiesCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queuePropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queuePropertiesCount, queueFamilyProperties.data());

	for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
	{
		VkBool32 supported = VK_FALSE;
		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, *_surface, &supported), "Failed to Get Physical Device Surface Support KHR !");
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supported)
		{
			m_queueIndex = i;
			i = static_cast<uint32_t>(queueFamilyProperties.size());
		}
	}

	if (m_queueIndex == ~0)
	{
		// TODO: print error message with logger
	}

	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	physicalDeviceFeatures.shaderInt16 = VK_TRUE;
	physicalDeviceFeatures.sampleRateShading = VK_TRUE;
	physicalDeviceFeatures.drawIndirectFirstInstance = VK_TRUE;
	physicalDeviceFeatures.multiDrawIndirect = VK_TRUE;

	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	descriptorIndexingFeatures.pNext = VK_NULL_HANDLE;
	descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
	descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
	descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
	descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
	descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
	descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
	descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;

	VkPhysicalDeviceVulkan11Features vulkan11Features{};
	vulkan11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	vulkan11Features.pNext = &descriptorIndexingFeatures;
	vulkan11Features.storageBuffer16BitAccess = VK_TRUE;
	vulkan11Features.uniformAndStorageBuffer16BitAccess = VK_TRUE;
	vulkan11Features.shaderDrawParameters = VK_TRUE;

	VkPhysicalDeviceVulkan13Features vulkan13Features{};
	vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vulkan13Features.synchronization2 = VK_TRUE;
	vulkan13Features.dynamicRendering = VK_TRUE;
	vulkan13Features.pNext = &vulkan11Features;

	VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicStateFeatures{};
	dynamicStateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
	dynamicStateFeatures.extendedDynamicState = VK_TRUE;
	dynamicStateFeatures.pNext = &vulkan13Features;

	VkPhysicalDeviceFeatures2 deviceFeatures{};
	deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures.features = physicalDeviceFeatures;
	deviceFeatures.pNext = &dynamicStateFeatures;

	const float queuePriority = 0.5f;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueFamilyIndex = m_queueIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = &deviceFeatures;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredDeviceExtension.size());
	deviceCreateInfo.ppEnabledExtensionNames = m_requiredDeviceExtension.data();

	VK_CHECK(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device), "Failed to create the Device !");

	vkGetDeviceQueue(m_device, m_queueIndex, 0, &m_graphicsQueue);
}

bool VK_Device::IsDeviceSuitable(const VkPhysicalDevice& _physicalDevice)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

	const bool suppVk14 = deviceProperties.apiVersion >= VK_API_VERSION_1_4;

	uint32_t queuePropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queuePropertiesCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queuePropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queuePropertiesCount, queueFamilyProperties.data());

	bool suppGraphics = false;
	for (size_t i = 0; i < queueFamilyProperties.size(); ++i)
	{
		if (!!(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			suppGraphics = true;
			i = queueFamilyProperties.size();
		}
	}

	uint32_t deviceExtensionPropertiesCount = 0;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &deviceExtensionPropertiesCount, nullptr), "Failed to Enumerate Device Extension Properties !");
	std::vector<VkExtensionProperties> extensionProperties(deviceExtensionPropertiesCount);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &deviceExtensionPropertiesCount, extensionProperties.data()), "Failed to Enumerate Device Extension Properties !");

	bool suppAllRequiredExtensions = false;
	for (size_t i = 0; i < m_requiredDeviceExtension.size(); ++i)
	{
		for (size_t j = 0; j < extensionProperties.size(); ++j)
		{
			if (strcmp(m_requiredDeviceExtension[i], extensionProperties[j].extensionName) == 0)
			{
				suppAllRequiredExtensions = true;
				j = extensionProperties.size();
			}
		}

		if (suppAllRequiredExtensions)
		{
			i = m_requiredDeviceExtension.size();
		}
	}

	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;

	VkPhysicalDeviceVulkan11Features features11{};
	features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	features11.pNext = &descriptorIndexingFeatures;

	VkPhysicalDeviceVulkan13Features features13{};
	features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features13.pNext = &features11;

	VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamicStateFeatures{};
	dynamicStateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
	dynamicStateFeatures.pNext = &features13;

	VkPhysicalDeviceFeatures2 deviceFeatures;
	deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures.pNext = &dynamicStateFeatures;
	vkGetPhysicalDeviceFeatures2(_physicalDevice, &deviceFeatures);

	const bool suppBindless = descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing &&
		descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind &&
		descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing &&
		descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind &&
		descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing &&
		descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind &&
		descriptorIndexingFeatures.descriptorBindingPartiallyBound &&
		descriptorIndexingFeatures.runtimeDescriptorArray;

	const bool suppRequieredFeatures = deviceFeatures.features.samplerAnisotropy && deviceFeatures.features.multiDrawIndirect &&
		deviceFeatures.features.multiDrawIndirect &&
		deviceFeatures.features.shaderInt16 && deviceFeatures.features.sampleRateShading &&
		features11.storageBuffer16BitAccess && features11.uniformAndStorageBuffer16BitAccess &&
		features11.shaderDrawParameters && features13.dynamicRendering &&
		features13.synchronization2 && dynamicStateFeatures.extendedDynamicState;

	return suppVk14 && suppGraphics && suppAllRequiredExtensions && suppRequieredFeatures && suppBindless;
}