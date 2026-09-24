#include "VK_RenderWindow.h"

#include "VK_SwapChain.h"
#include "VK_DepthResources.h"
#include "../../Asset/Texture.h"

void VK_RenderWindow::Init(VK_Device* _device, VK_Instance* _instance, bool _useDepth)
{
	LLYN_ASSERT(_device == nullptr || _instance == nullptr);

	m_swapChain = new VK_SwapChain;
	m_swapChain->Init(_device, _instance);

	const VkExtent2D* extent = m_swapChain->GetExtent();

	m_area.offset = { 0, 0 };
	m_area.extent = *extent;

	m_scissor.offset = { 0, 0 };
	m_scissor.extent = *extent;

	m_viewport.x = 0;
	m_viewport.y = static_cast<float>(extent->height);
	m_viewport.width = static_cast<float>(extent->width);
	m_viewport.height = -static_cast<float>(extent->height);
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	if (_useDepth)
	{
		m_depthResources = new VK_DepthResources;
		m_depthResources->Init(_device, *m_swapChain->GetExtent());
	}
}

void VK_RenderWindow::Cleanup(VK_Device* _device)
{

}

void VK_RenderWindow::Clear(VkCommandBuffer _cmdBuff, uint32_t _imageIndex)
{
	Texture::TransitionImageLayout(_cmdBuff, m_swapChain->GetImage(_imageIndex), 1,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_NONE, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);

	if (m_depthResources != nullptr)
	{
		Texture::TransitionImageLayout(_cmdBuff, m_depthResources->GetImage(), 1,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
			VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);
	}
}

void VK_RenderWindow::BeginRendering(VkCommandBuffer _cmdBuff, uint32_t _imageIndex, VkClearValue* _colorClear, VkClearValue* _depthValue)
{
	VkImageView colorView = *m_swapChain->GetImageView(_imageIndex);
	VkImageView depthView = m_depthResources ? *m_depthResources->GetImageView() : VK_NULL_HANDLE;

	BeginRendering(_cmdBuff, colorView, _colorClear, depthView, _depthValue);
}

void VK_RenderWindow::BeginRendering(VkCommandBuffer _cmdBuff, VkImageView _imageView, VkClearValue* _clearColor,
	VkImageView _depthView, VkClearValue* _depthValue)
{
	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.pNext = VK_NULL_HANDLE;
	colorAttachment.imageView = _imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	colorAttachment.resolveImageView = VK_NULL_HANDLE;
	colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.loadOp = _clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	if (_clearColor != nullptr)
	{
		colorAttachment.clearValue = *_clearColor;
	}

	VkRenderingAttachmentInfo depthAttachment{};
	if (_depthView != VK_NULL_HANDLE)
	{
		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.pNext = VK_NULL_HANDLE;
		depthAttachment.imageView = _depthView;
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
		depthAttachment.resolveImageView = VK_NULL_HANDLE;
		depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.loadOp = _depthValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		if (_depthValue != nullptr)
		{
			depthAttachment.clearValue = *_depthValue;
		}
	}

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.pNext = VK_NULL_HANDLE;
	renderingInfo.renderArea = m_area;
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = (_depthView != VK_NULL_HANDLE) ? &depthAttachment : VK_NULL_HANDLE;

	vkCmdBeginRendering(_cmdBuff, &renderingInfo);
}

void VK_RenderWindow::EndRendering(VkCommandBuffer _cmdBuff)
{
	vkCmdEndRendering(_cmdBuff);
}

void VK_RenderWindow::Display(VkCommandBuffer _cmdBuff, uint32_t _imageIndex)
{
	Texture::TransitionImageLayout(_cmdBuff, m_swapChain->GetImage(_imageIndex), 1,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT);
}

const VK_SwapChain* VK_RenderWindow::GetSwapChain()
{
	return m_swapChain;
}

VkViewport* VK_RenderWindow::GetWiewport()
{
	return &m_viewport;
}

VkRect2D* VK_RenderWindow::GetScissor()
{
	return &m_scissor;
}
