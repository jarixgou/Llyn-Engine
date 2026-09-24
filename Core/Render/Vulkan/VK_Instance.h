#ifndef VK_INSTANCE__H
#define VK_INSTANCE__H

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class VK_Instance
{
private:
	VkInstance m_instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	
	uint32_t m_apiVersion = ~0;

	static const std::vector<char const*> m_validationLayers;
public:
	void Init(const std::string& _name);
	void Cleanup();

	const VkInstance* GetInstance() const;
	const VkSurfaceKHR* GetSurface() const;

	uint32_t GetAPIVersion() const;
private:
	void CreateInstance(const std::string& _name);
	static std::vector<char const*> GetRequiredExtensions();
	static void CheckUnsupportedExtensions(std::vector<char const*>& _requiredExtensions);
	static bool CheckValidationLayerSupport();
	void SetupDebugMessenger();
	static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo);
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance _instance,
	                                             const VkDebugUtilsMessengerCreateInfoEXT* _createInfo,
	                                             const VkAllocationCallbacks* _allocator,
	                                             VkDebugUtilsMessengerEXT* _debugMessenger);
	static void DestroyDebugUtilsMessengerEXT(VkInstance _instance, VkDebugUtilsMessengerEXT _debugMessenger,
	                                          const VkAllocationCallbacks* _allocator);
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT _severity,
		VkDebugUtilsMessageTypeFlagsEXT _type,
		const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData,
		void* _pUserData);
	void CreateSurface();
};

#endif