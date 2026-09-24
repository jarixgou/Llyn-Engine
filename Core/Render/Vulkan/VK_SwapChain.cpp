#include "VK_SwapChain.h"

#include <iostream>

#include "VK_Device.h"
#include "VK_Instance.h"
#include "../Window.h"
#include "../../Logger.h"
#include "../../Asset/Texture.h"

#include "../../Vector/Vec2.h"

void VK_SwapChain::Init(const VK_Device* _device, const VK_Instance* _instance)
{
	LLYN_ASSERT(_device != nullptr || _instance != nullptr);

	CreateSwapChain(_device, _instance);
	CreateImageViews(_device);
}

void VK_SwapChain::Cleanup(VK_Device* _device)
{
	LOGGER_INFO("Destroying VK_SwapChain");
	vkDestroySwapchainKHR(*_device->GetDevice(), m_swapChain, nullptr);
	LOGGER_INFO("VkSwapchainKHR is destroyed");
}

void VK_SwapChain::Recreate()
{
}

const VkImage* VK_SwapChain::GetImage(uint32_t _imageIndex) const
{
	LLYN_ASSERT(_imageIndex < MAX_FRAMES_IN_FLIGHT);
	return &m_images[_imageIndex];
}

const VkImageView* VK_SwapChain::GetImageView(uint32_t _imageIndex) const
{
	LLYN_ASSERT(_imageIndex < MAX_FRAMES_IN_FLIGHT);
	return &m_imageViews[_imageIndex];
}

VkFormat VK_SwapChain::GetFormat() const
{
	return m_surfaceFormat.format;
}

const VkSwapchainKHR* VK_SwapChain::GetSwapChain() const
{
	return &m_swapChain;
}

const VkExtent2D* VK_SwapChain::GetExtent() const
{
	return &m_extent;
}

const VkSurfaceFormatKHR* VK_SwapChain::GetSwapChainSurface() const
{
	return &m_surfaceFormat;
}

size_t VK_SwapChain::GetImageCount() const
{
	return m_images.size();
}

uint32_t VK_SwapChain::GetMinImageCount() const
{
	return m_minImageCount;
}

void VK_SwapChain::CreateSwapChain(const VK_Device* _device, const VK_Instance* _instance)
{
	LLYN_ASSERT(_device != nullptr || _instance != nullptr)

		const VkPhysicalDevice* physicalDevice = _device->GetPhycicalDevice();
	const VkDevice* device = _device->GetDevice();
	const VkSurfaceKHR* surface = _instance->GetSurface();

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physicalDevice, *surface, &surfaceCapabilities), "Failed to Get Physical Device Surface Capabilities KHR !");
	m_extent = ChooseExtent(surfaceCapabilities, Window::Get().GetSize());

	m_minImageCount = ChooseMinImageCount(surfaceCapabilities);

	uint32_t surfaceFormatCount = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *surface, &surfaceFormatCount, nullptr), "Failed to Get Physical Device Surface Format KHR !");
	std::vector<VkSurfaceFormatKHR> availableFormats(surfaceFormatCount);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(*physicalDevice, *surface, &surfaceFormatCount, availableFormats.data()), "Failed to Get Physical Device Surface Format KHR !");
	m_surfaceFormat = ChooseFormat(availableFormats);

	uint32_t presentModeCount = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface, &presentModeCount, nullptr), "Failed to Get Physical Device Surface Present Mode KHR !");
	std::vector<VkPresentModeKHR> availablePresentMode(presentModeCount);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(*physicalDevice, *surface, &presentModeCount, availablePresentMode.data()), "Failed to Get Physical Device Surface Present Mode KHR");
	const VkPresentModeKHR presentMode = ChoosePresentMode(availablePresentMode);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = VK_NULL_HANDLE;
	createInfo.flags = 0;
	createInfo.surface = *surface;
	createInfo.minImageCount = m_minImageCount;
	createInfo.imageFormat = m_surfaceFormat.format;
	createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
	createInfo.imageExtent = m_extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VK_CHECK(vkCreateSwapchainKHR(*device, &createInfo, nullptr, &m_swapChain), "Failed to create SwapChain KHR !");

	VK_CHECK(vkGetSwapchainImagesKHR(*device, m_swapChain, &m_imageCount, nullptr), "Failed to get SwapChain Images KHR !");
	m_images.resize(m_imageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(*device, m_swapChain, &m_imageCount, m_images.data()), "Failed to get SwapChain Images KHR !");
}

void VK_SwapChain::CreateImageViews(const VK_Device* _device)
{
	assert(m_imageViews.empty());

	m_imageViews.reserve(m_images.size());
	for (size_t i = 0; i < m_images.size(); ++i)
	{
		VkImageView imageView = Texture::CreateImageView(1, VK_IMAGE_VIEW_TYPE_2D, &m_images[i],
			m_surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

		m_imageViews.emplace_back(imageView);
	}
}

VkExtent2D VK_SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR& _capabilities, const Vec2i& _windowSize)
{
	if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return _capabilities.currentExtent;
	}

	return {
		std::clamp<uint32_t>(_windowSize.x, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(_windowSize.y, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height)
	};
}

uint32_t VK_SwapChain::ChooseMinImageCount(const VkSurfaceCapabilitiesKHR& _capabilities)
{
	auto minImageCount = (std::max)(3u, _capabilities.minImageCount);
	if (0 < _capabilities.maxImageCount && _capabilities.maxImageCount < minImageCount)
	{
		minImageCount = _capabilities.maxImageCount;
	}
	return minImageCount;
}

VkSurfaceFormatKHR VK_SwapChain::ChooseFormat(const std::vector<VkSurfaceFormatKHR>& _availableFormats)
{
	for (size_t i = 0; i < _availableFormats.size(); ++i)
	{
		if (_availableFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB &&
			_availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return _availableFormats[i];
		}
	}

	return _availableFormats[0];
}

VkPresentModeKHR VK_SwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& _presentModes)
{
	for (size_t i = 0; i < _presentModes.size(); ++i)
	{
		if (_presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return _presentModes[i];
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
