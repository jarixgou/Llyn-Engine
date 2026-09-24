#include "VK_FrameSync.h"

#include <cassert>

#include "VK_Device.h"
#include "VK_SwapChain.h"
#include "VK_Buffer.h"
#include "../../Logger.h"

void VK_FrameSync::Init(const VK_Device* _device, const VK_SwapChain* _swapChain)
{
	LLYN_ASSERT(_device != nullptr || _swapChain != nullptr);

	m_cmdPool = VK_Buffer::CreateCommandPool();
	m_cmdBuffers = VK_Buffer::CreateCommandBuffer(m_cmdPool, MAX_FRAMES_IN_FLIGHT);
	CreateSyncObjects(_device, _swapChain->GetImageCount());
}

void VK_FrameSync::Cleanup(const VK_Device* _device)
{
	LLYN_ASSERT(_device != nullptr);

	LOGGER_INFO("Destroying VK_FrameSync");

	vkDestroyCommandPool(*_device->GetDevice(), m_cmdPool, nullptr);
	LOGGER_INFO("VkCommandPool is destroyed");

	for (size_t i = 0; i < m_presentCompleteSemaphores.size(); ++i)
	{
		vkDestroySemaphore(*_device->GetDevice(), m_presentCompleteSemaphores[i], nullptr);
	}
	m_presentCompleteSemaphores.clear();
	LOGGER_INFO("Present complete semaphores is destroyed");

	for (size_t i = 0; i < m_renderFinishedSemaphores.size(); ++i)
	{
		vkDestroySemaphore(*_device->GetDevice(), m_renderFinishedSemaphores[i], nullptr);
	}
	m_renderFinishedSemaphores.clear();
	LOGGER_INFO("Render finished semaphores is destroyed");

	for (size_t i = 0; i < m_inFlightFences.size(); ++i)
	{
		vkDestroyFence(*_device->GetDevice(), m_inFlightFences[i], nullptr);
	}
	m_inFlightFences.clear();
	LOGGER_INFO("In flight fences is destroyed !");
}

std::pair<uint32_t, VkCommandBuffer> VK_FrameSync::Begin(const VK_Device* _device, const VK_SwapChain* _swapChain)
{
	LLYN_ASSERT(_device != nullptr || _swapChain != nullptr);

	VK_CHECK(vkWaitForFences(*_device->GetDevice(), 1, &m_inFlightFences[m_frameIndex], VK_TRUE, UINT64_MAX),
		"Failed to wait fence");

	uint32_t imageIndex = 0;
	const VkResult swapChainResult = vkAcquireNextImageKHR(*_device->GetDevice(), *_swapChain->GetSwapChain(),
		UINT64_MAX, m_presentCompleteSemaphores[m_frameIndex], VK_NULL_HANDLE, &imageIndex);

	if (swapChainResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//_swapChain.Recreate();
		return { imageIndex, *GetCommandBuffer()};
	}
	else
	{
		VK_CHECK(swapChainResult, "Failed to acquire next image !");
	}
	return { imageIndex, *GetCommandBuffer() };
}

void VK_FrameSync::Reset(const VK_Device* _device)
{
	VK_CHECK(vkResetFences(*_device->GetDevice(), 1, &m_inFlightFences[m_frameIndex]), "Failed to reset fences !");
	VK_CHECK(vkResetCommandBuffer(*GetCommandBuffer(), 0),
		"Failed to reset the command buffer !");
}

void VK_FrameSync::End(const VK_Device* _device, const VK_SwapChain* _swapChain, uint32_t _imageIndex, std::vector<VkCommandBuffer> _cmdBuffs)
{
	LLYN_ASSERT(_device != nullptr || _swapChain != nullptr);

	const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = VK_NULL_HANDLE;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_presentCompleteSemaphores[m_frameIndex];
	submitInfo.commandBufferCount = static_cast<uint32_t>(_cmdBuffs.size());
	submitInfo.pCommandBuffers = _cmdBuffs.data();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_renderFinishedSemaphores[_imageIndex];

	VK_CHECK(vkQueueSubmit(*_device->GetGraphicsQueue(), 1, &submitInfo, *GetFence()),
		"Failed to submit graphics queue !");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = VK_NULL_HANDLE;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[_imageIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = _swapChain->GetSwapChain();
	presentInfo.pImageIndices = &_imageIndex;

	const VkResult submitResult = vkQueuePresentKHR(*_device->GetGraphicsQueue(), &presentInfo);

	if (submitResult == VK_SUBOPTIMAL_KHR || submitResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//_swapChain->Recreate();
	}
	else
	{
		VK_CHECK(submitResult, "Failed to present graphics queue !");
	}

	m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

const VkCommandPool* VK_FrameSync::GetCommandPool() const
{
	return &m_cmdPool;
}

VkCommandBuffer* VK_FrameSync::GetCommandBuffer()
{
	return &m_cmdBuffers[m_frameIndex];
}

const VkFence* VK_FrameSync::GetFence()
{
	return &m_inFlightFences[m_frameIndex];
}

const VkSemaphore* VK_FrameSync::GetPresentCompleteSemaphore()
{
	return &m_presentCompleteSemaphores[m_frameIndex];
}

const VkSemaphore* VK_FrameSync::GetRenderFinishedSemaphore()
{
	return &m_renderFinishedSemaphores[m_frameIndex];
}

uint32_t VK_FrameSync::GetFrameIndex()
{
	return m_frameIndex;
}

void VK_FrameSync::CreateSyncObjects(const VK_Device* _device, const size_t _swapChainImageCount)
{
	LLYN_ASSERT(_device != nullptr);

	m_presentCompleteSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(_swapChainImageCount); // was MAX_FRAMES_IN_FLIGHT
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		VK_CHECK(vkCreateSemaphore(*_device->GetDevice(), &semaphoreCreateInfo,
			VK_NULL_HANDLE, &m_presentCompleteSemaphores[i]), "Failed to create semaphore");
		VK_CHECK(vkCreateFence(*_device->GetDevice(), &fenceCreateInfo,
			VK_NULL_HANDLE, &m_inFlightFences[i]), "Failed to create fence");
	}

	for (size_t i = 0; i < _swapChainImageCount; ++i)
	{
		VK_CHECK(vkCreateSemaphore(*_device->GetDevice(), &semaphoreCreateInfo,
			VK_NULL_HANDLE, &m_renderFinishedSemaphores[i]), "Failed to create semaphore");
	}
}
