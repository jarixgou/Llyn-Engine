#ifndef VK_FRAME_SYNC__H
#define VK_FRAME_SYNC__H

#include <vector>
#include <vulkan/vulkan.h>

#include "../../LlynCore.h"

class VK_FrameSync
{
private:
	VkCommandPool m_cmdPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_cmdBuffers;

	std::vector<VkSemaphore> m_presentCompleteSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;

	uint32_t m_frameIndex = 0;
public:
	void Init(const VK_Device* _device, const VK_SwapChain* _swapChain);
	void Cleanup(const VK_Device* _device);

	std::pair<uint32_t, VkCommandBuffer> Begin(const VK_Device* _device, const VK_SwapChain* _swapChain);
	void Reset(const VK_Device* _device);
	void End(const VK_Device* _device, const VK_SwapChain* _swapChain, uint32_t _imageIndex, std::vector<VkCommandBuffer> _cmdBuffs);

	const VkCommandPool* GetCommandPool() const;
	VkCommandBuffer* GetCommandBuffer();
	const VkFence* GetFence();
	const VkSemaphore* GetPresentCompleteSemaphore();
	const VkSemaphore* GetRenderFinishedSemaphore();
	uint32_t GetFrameIndex();
private:
	void CreateSyncObjects(const VK_Device* _device, size_t _swapChainImageCount);
};

#endif