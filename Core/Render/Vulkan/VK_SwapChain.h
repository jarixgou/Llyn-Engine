#ifndef VK_SWAPCHAIN__H
#define VK_SWAPCHAIN__H
#include <vector>

#include <vulkan/vulkan.h>

#include "../../LlynCore.h"

#include "../../Vector/FwdVec2.h"

class VK_SwapChain
{
private:
	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	VkSurfaceFormatKHR m_surfaceFormat = {};
	VkExtent2D m_extent = {};

	uint32_t m_minImageCount = 0;
	uint32_t m_imageCount = 0;
public:	// Public function
	void Init(const VK_Device* _device, const VK_Instance* _instance);
	void Cleanup(VK_Device* _device);

	void Recreate();

	const VkImage* GetImage(uint32_t _imageIndex) const;
	const VkImageView* GetImageView(uint32_t _imageIndex) const;

	VkFormat GetFormat() const;
	const VkSwapchainKHR* GetSwapChain() const;
	const VkExtent2D* GetExtent() const;
	const VkSurfaceFormatKHR* GetSwapChainSurface() const;
	size_t GetImageCount() const;

	uint32_t GetMinImageCount() const;
private: // Private function
	void CreateSwapChain(const VK_Device* _device, const VK_Instance* _instance);
	void CreateImageViews(const VK_Device* _device);

	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& _capabilities, const Vec2i& _windowSize);
	static uint32_t ChooseMinImageCount(const VkSurfaceCapabilitiesKHR& _capabilities);
	static VkSurfaceFormatKHR ChooseFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& _presentModes);
};

#endif