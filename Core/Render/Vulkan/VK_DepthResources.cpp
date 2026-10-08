#include "VK_DepthResources.h"

#include "VK_Device.h"
#include "VK_SwapChain.h"
#include "../../Asset/TextureConfig.h"
#include "../../Utils/VK_Utils.h"
#include "../../Asset/Texture.h"
#include "../../Vector/Vec2.h"

#include "../../Logger.h"

void VK_DepthResources::Init(VkExtent2D _extent)
{
	const VkFormat depthFormat = FinDepthFormat(&VK_Device::Get());

	m_image.Init(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, IMAGE_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, Vec2u(_extent.width, _extent.height));
}

void VK_DepthResources::Cleanup()
{
	LOGGER_INFO("Destroying VK_DepthResources");
	m_image.Cleanup();
}

VK_Image* VK_DepthResources::GetImage()
{
	return &m_image;
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
