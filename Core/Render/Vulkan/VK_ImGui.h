#ifndef VK_IMGUI__H
#define VK_IMGUI__H

#include <vector>

#include <vulkan/vulkan.h>
#include "../../LlynCore.h"
#include "../../Vector/Vec2.h"

class VK_ImGui
{
private:
	Vec2u m_frameBufferSize = { 1920, 1080 };
	std::vector<VkCommandBuffer> m_cmdBuffs;
	VkDescriptorPool m_desscriptorPool = VK_NULL_HANDLE;
public:
	void Init(const VK_Instance* _instance, const VK_Device* _device, const VK_SwapChain* _swapChain, const VK_FrameSync* _frameSync);
	void Cleanup(const VK_Device* _device, VkCommandPool _cmdPool);

	VkCommandBuffer Draw(VK_RenderWindow* _renderWindow, uint32_t _frameIndex, uint32_t _imageIndex);
private:
	void InitImGui(const VK_Instance* _instance, const VK_Device* _device, const VK_SwapChain* _swapChain, const VK_FrameSync* _frameSync);
	void CreateDescriptorPool(const VK_Device* _device);
};

#endif