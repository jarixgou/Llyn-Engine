#include "VK_DepthResources.h"

#include "VK_Device.h"
#include "VK_SwapChain.h"
#include "../../Utils/VK_Utils.h"
#include "../../Asset/Texture.h"
#include "../../Vector/Vec2.h"

#include "../../Logger.h"

void VK_DepthResources::Init(const VK_Device* _device, VkExtent2D _extent)
{
	LLYN_ASSERT(_device != nullptr);

	const VkFormat depthFormat = FinDepthFormat(_device);

	std::tie(m_image, m_imageMemory) = Texture::CreateImage(
		Vec2<uint32_t>(_extent.width, _extent.height), 1, VK_IMAGE_TYPE_2D,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_imageView = Texture::CreateImageView(1, VK_IMAGE_VIEW_TYPE_2D, &m_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VK_DepthResources::Cleanup(const VK_Device* _device) const
{
	LOGGER_INFO("Destroying VK_DepthResources");

	vkDestroyImage(*_device->GetDevice(), m_image, nullptr);
	LOGGER_INFO("VkImage is destroyed");

	vkDestroyImageView(*_device->GetDevice(), m_imageView, nullptr);
	LOGGER_INFO("VkImageView is destroyed");

	vkFreeMemory(*_device->GetDevice(), m_imageMemory, nullptr);
	LOGGER_INFO("VkDeviceMemory is free");
}

const VkImage* VK_DepthResources::GetImage()
{
	return &m_image;
}

const VkImageView* VK_DepthResources::GetImageView()
{
	return &m_imageView;
}

VkFormat VK_DepthResources::FinDepthFormat(const VK_Device* _device)
{
	if (_device == nullptr)
	{
		// TODO: print error message with logger
	}

	return Utils::FindSupportedFormat( _device,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
