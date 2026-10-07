#include "VK_Image.h"

#include <assert.h>

#include "VK_Device.h"
#include "../../Vector/Vec2.h"
#include "VK_StageAndAccess.h"
#include "../../Asset/TextureConfig.h"
#include "../../Logger.h"

void VK_Image::Init(VkFormat _format, VkImageLayout _layout, const ImageType& _type, VkImageAspectFlags _aspect, VkImageUsageFlags _usage, uint32_t _mipsLevel, const
                    Vec2u& _textureSize, bool _swapChain)
{
	m_format = _format;
	m_layout = _layout;

	m_type = GetType(_type);
	m_viewType = GetViewType(_type);
	m_mipLevels = _mipsLevel;

	m_swapChain = _swapChain;

	if (!m_swapChain)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext = VK_NULL_HANDLE;
		imageInfo.format = _format;
		imageInfo.extent = { _textureSize.x, _textureSize.y, 1 };
		imageInfo.mipLevels = _mipsLevel;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = _usage;
		VK_VmaAllocatorWrapper::Get().CreateImage(imageInfo, VMA_MEMORY_USAGE_AUTO, 0, m_image, m_allocation);
	}
	CreateView();
}

void VK_Image::Cleanup()
{
	if (m_imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(*VK_Device::Get().GetDevice(), m_imageView, VK_NULL_HANDLE);
	}

	if (!m_swapChain && m_image != VK_NULL_HANDLE)
	{
		VK_VmaAllocatorWrapper::Get().DestroyImage(m_image, m_allocation);
	}
}

void VK_Image::CreateView()
{
	VkImageSubresourceRange subresource{};
	subresource.aspectMask = m_aspect;
	subresource.baseMipLevel = 0;
	subresource.levelCount = m_mipLevels;
	subresource.baseArrayLayer = 0;
	subresource.layerCount = 1;

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext = VK_NULL_HANDLE;
	viewInfo.image = m_image;
	viewInfo.viewType = m_viewType;
	viewInfo.format = m_format;
	viewInfo.subresourceRange = subresource;

	VK_CHECK(vkCreateImageView(*VK_Device::Get().GetDevice(), &viewInfo, VK_NULL_HANDLE, &m_imageView),
		"Failed to create image view ! ");
}

void VK_Image::TransitionLayout(VkCommandBuffer _cmdBuff, VkImageLayout _newLayout, uint32_t _baseMipLevel)
{
	TransitionLayout(_cmdBuff, m_layout, _newLayout, _baseMipLevel);
}

void VK_Image::TransitionLayout(VkCommandBuffer _cmdBuff, VkImageLayout _oldLayout, VkImageLayout _newLayout,
	uint32_t _baseMipLevel)
{
	ImageBarrier(_cmdBuff, _oldLayout, _newLayout, 1, _baseMipLevel);
	m_layout = _newLayout;
}

bool VK_Image::HasStencilComponent(VkFormat _format)
{
	return _format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
		_format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkImageType VK_Image::GetType(ImageType _type)
{
	switch (_type)
	{
	case IMAGE_TYPE_1D:
		return VK_IMAGE_TYPE_1D;
		break;
	case IMAGE_TYPE_2D:
		return VK_IMAGE_TYPE_2D;
		break;
	case IMAGE_TYPE_3D:
		return VK_IMAGE_TYPE_3D;
		break;
	case IMAGE_TYPE_CUBE:
		return VK_IMAGE_TYPE_2D;
		break;
	case IMAGE_TYPE_MAX_ENUM:
		LOGGER_WARNING("Wrong image type ! ");
		return VK_IMAGE_TYPE_2D;
		break;
	}

	LOGGER_WARNING("Don't find the right image type");
	return VK_IMAGE_TYPE_2D;
}

VkImageViewType VK_Image::GetViewType(ImageType _type)
{
	switch (_type) {
	case IMAGE_TYPE_1D:
		return VK_IMAGE_VIEW_TYPE_1D;
		break;
	case IMAGE_TYPE_2D:
		return VK_IMAGE_VIEW_TYPE_2D;
		break;
	case IMAGE_TYPE_3D:
		return VK_IMAGE_VIEW_TYPE_3D;
		break;
	case IMAGE_TYPE_CUBE:
		return VK_IMAGE_VIEW_TYPE_CUBE;
		break;
	case IMAGE_TYPE_MAX_ENUM:
		LOGGER_WARNING("Wrong image type for image view !");
		break;
	}

	LOGGER_WARNING("Don't find the right image type for image view ! ");
	return VK_IMAGE_VIEW_TYPE_2D;
}

void VK_Image::ImageBarrier(VkCommandBuffer _cmdBuff, VkImageLayout _oldLayout, VkImageLayout _newLayout,
                            uint32_t _layerCount, uint32_t _baseMipsLevel)
{
	assert(_layerCount > 0);

	VK_StageAndAccess stageAndAccess = GetStageAndAccess(_oldLayout, _newLayout);

	VkImageSubresourceRange subresource{};
	subresource.aspectMask = GetImageAspect(_newLayout);
	subresource.baseMipLevel = _baseMipsLevel;
	subresource.levelCount = m_mipLevels;
	subresource.baseArrayLayer = 0;
	subresource.layerCount = _layerCount;

	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = VK_NULL_HANDLE;
	barrier.srcStageMask = stageAndAccess.srcStage;
	barrier.srcAccessMask = stageAndAccess.srcAccess;
	barrier.dstStageMask = stageAndAccess.dstStage;
	barrier.dstAccessMask = stageAndAccess.dstAccess;
	barrier.oldLayout = _oldLayout;
	barrier.newLayout = _newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_image;
	barrier.subresourceRange = subresource;

	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.pNext = VK_NULL_HANDLE;
	dependencyInfo.dependencyFlags = 0;
	dependencyInfo.memoryBarrierCount = 0;
	dependencyInfo.pMemoryBarriers = VK_NULL_HANDLE;
	dependencyInfo.bufferMemoryBarrierCount = 0;
	dependencyInfo.pBufferMemoryBarriers = VK_NULL_HANDLE;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(_cmdBuff, &dependencyInfo);
}

VkImageAspectFlags VK_Image::GetImageAspect(VkImageLayout _layout)
{
	VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_NONE;

	if (_layout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL ||
		m_format == VK_FORMAT_D16_UNORM || m_format == VK_FORMAT_X8_D24_UNORM_PACK32 ||
		m_format == VK_FORMAT_D32_SFLOAT || m_format == VK_FORMAT_S8_UINT ||
		m_format == VK_FORMAT_D16_UNORM_S8_UINT || m_format == VK_FORMAT_D24_UNORM_S8_UINT)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(m_format))
		{
			imageAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	return imageAspect;
}

VK_StageAndAccess VK_Image::GetStageAndAccess(VkImageLayout _oldLayout,
	VkImageLayout _newLayout)
{
	VK_StageAndAccess stageAndAccess{};

	if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_NONE;
		stageAndAccess.srcAccess = VK_ACCESS_2_NONE;

		if (_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_GENERAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
				VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
		else
		{
			LOGGER_CRITICAL("Unknow barrier case 1 !");
		}
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		stageAndAccess.srcAccess = VK_ACCESS_2_SHADER_READ_BIT;

		if (_newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
				VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
				VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else
		{
			LOGGER_CRITICAL("Unknow barrier case 2 !");
		}
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		stageAndAccess.srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;

		if (_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_GENERAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_NONE;
		}
		else
		{
			LOGGER_CRITICAL("Unknow barrier case 3 !");
		}
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		stageAndAccess.srcAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

		if (_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_NONE;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_GENERAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_TRANSFER_READ_BIT;
		}
		else
		{
			LOGGER_CRITICAL("Unknow barrier case 4 !");
		}
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_GENERAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		stageAndAccess.srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;

		if (_newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_NONE;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT |
				VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_GENERAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT |
				VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;
		}
		else
		{
			LOGGER_CRITICAL("Unknow barrier case 5 !");
		}
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
			VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
		stageAndAccess.srcAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		if (_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			// Multi-mesh accumulation without clearing depth between draws
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else if (_newLayout == VK_IMAGE_LAYOUT_UNDEFINED)
		{
			// NOTE: Avoid transitioning images into UNDEFINED layout mid-frame. 
			// If this is used for frame boundaries, ensure you use attachment load ops to handle the clear safely.
			stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
			stageAndAccess.dstAccess = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else
		{
			LOGGER_CRITICAL("Unknow barrier case 6 !");
		}
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
		_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		stageAndAccess.srcAccess = VK_ACCESS_2_TRANSFER_READ_BIT;
		stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		stageAndAccess.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (_oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
		_newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		stageAndAccess.srcStage = VK_PIPELINE_STAGE_2_NONE;
		stageAndAccess.srcAccess = VK_ACCESS_2_NONE;
		stageAndAccess.dstStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		stageAndAccess.dstAccess = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else
	{
		LOGGER_CRITICAL("Unknow barrier case 7 !");
	}

	return stageAndAccess;
}
