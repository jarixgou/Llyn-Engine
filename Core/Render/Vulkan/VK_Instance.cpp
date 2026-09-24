#include "VK_Instance.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../Window.h"
#include "../../LlynCore.h"
#include "../../Logger.h"

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

const std::vector<char const*> VK_Instance::m_validationLayers =
{
	"VK_LAYER_KHRONOS_validation",
};

void VK_Instance::Init(const std::string& _name)
{
	CreateInstance(_name);
	SetupDebugMessenger();
	CreateSurface();
}

void VK_Instance::Cleanup()
{
	LOGGER_INFO("Destroying VK_Instance");

	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	LOGGER_INFO("VkSurfaceKHR is destroyed");

	vkDestroyInstance(m_instance, nullptr);
	LOGGER_INFO("VkInstance is destroyed");
}

const VkInstance* VK_Instance::GetInstance() const
{
	return &m_instance;
}

const VkSurfaceKHR* VK_Instance::GetSurface() const
{
	return &m_surface;
}

uint32_t VK_Instance::GetAPIVersion() const
{
	return m_apiVersion;
}

void VK_Instance::CreateInstance(const std::string& _name)
{
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		// TODO: print error message with logger
	}

	m_apiVersion = VK_API_VERSION_1_4;

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = _name.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Llyn";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = m_apiVersion;

	VkInstanceCreateInfo instanceInfo{};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> requiredExtensions = GetRequiredExtensions();
	CheckUnsupportedExtensions(requiredExtensions);

	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers)
	{
		instanceInfo.enabledLayerCount = m_validationLayers.size();
		instanceInfo.ppEnabledLayerNames = m_validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.pNext = nullptr;
	}

	VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &m_instance), "Failed to create Instance !");
}

std::vector<char const*> VK_Instance::GetRequiredExtensions()
{
	uint32_t glfwExtensionsCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
	extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

void VK_Instance::CheckUnsupportedExtensions(std::vector<char const*>& _requiredExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	for (size_t i = 0; i < _requiredExtensions.size(); ++i)
	{
		bool unsupportedExtensionFound = false;
		for (size_t j = 0; j < extensions.size(); ++j)
		{
			if (strcmp(_requiredExtensions[i], extensions[j].extensionName) == 0)
			{
				unsupportedExtensionFound = true;
				j = extensions.size();
			}
		}

		if (!unsupportedExtensionFound)
		{
			// TODO : print error with logger
			_requiredExtensions.erase(_requiredExtensions.begin() + i);
		}
	}
}

bool VK_Instance::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (size_t i = 0; i < m_validationLayers.size(); ++i)
	{
		bool layerFound = false;
		for (size_t j = 0; j < availableLayers.size(); ++j)
		{
			if (strcmp(m_validationLayers[i], availableLayers[j].layerName) == 0)
			{
				layerFound = true;
				j = availableLayers.size();
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

void VK_Instance::SetupDebugMessenger()
{
	if constexpr (!enableValidationLayers)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
	PopulateDebugMessengerCreateInfo(debugMessengerInfo);

	if (CreateDebugUtilsMessengerEXT(m_instance, &debugMessengerInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		// TODO: print error message with logger
	}

	/*vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{};
	debugUtilsMessengerCreateInfoEXT.setMessageSeverity(severityFlags);
	debugUtilsMessengerCreateInfoEXT.setMessageType(messageTypeFlags);
	debugUtilsMessengerCreateInfoEXT.setPfnUserCallback(&DebugCallBack);

	m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);*/
}

void VK_Instance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo)
{
	_createInfo = {};
	_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	_createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	_createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	_createInfo.pfnUserCallback = DebugCallBack;
}

VkResult VK_Instance::CreateDebugUtilsMessengerEXT(VkInstance _instance,
                                                   const VkDebugUtilsMessengerCreateInfoEXT* _createInfo, const VkAllocationCallbacks* _allocator,
                                                   VkDebugUtilsMessengerEXT* _debugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(_instance, _createInfo, _allocator, _debugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VK_Instance::DestroyDebugUtilsMessengerEXT(VkInstance _instance, VkDebugUtilsMessengerEXT _debugMessenger,
	const VkAllocationCallbacks* _allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(_instance, _debugMessenger, _allocator);
	}
}

VkBool32 VK_Instance::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT _severity,
                                    VkDebugUtilsMessageTypeFlagsEXT _type, const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, void* _pUserData)
{
	if (_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		// TODO: print message with logger
		//std::cerr << "Validation layer : type " << to_string(_type) << " msg : " << _pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;
}

void VK_Instance::CreateSurface()
{

	if (glfwCreateWindowSurface(m_instance, Window::Get().GetGLFWWindow(), nullptr, &m_surface) != VK_SUCCESS)
	{
		// TODO: print error with logger
	}
} 