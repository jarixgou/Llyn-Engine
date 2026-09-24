#ifndef VK_RENDER_WINDOW__H
#define VK_RENDER_WINDOW__H
#include <vulkan/vulkan_core.h>

#include "../../LlynCore.h"

class VK_RenderWindow
{
private:
	VK_SwapChain* m_swapChain = nullptr;
	VK_DepthResources* m_depthResources = nullptr;

	VkViewport m_viewport = {0};
	VkRect2D m_area = {0};
	VkRect2D m_scissor = {0};
public:
	void Init(VK_Device* _device, VK_Instance* _instance, bool _useDepth);
	void Cleanup(VK_Device* _device);

	void Clear(VkCommandBuffer _cmdBuff, uint32_t _imageIndex);

	void BeginRendering(VkCommandBuffer _cmdBuff, uint32_t _imageIndex, VkClearValue* _colorClear, VkClearValue* _depthValue);
	void BeginRendering(VkCommandBuffer _cmdBuff, VkImageView _imageView, VkClearValue* _clearColor,
		VkImageView _depthView, VkClearValue* _depthValue);
	void EndRendering(VkCommandBuffer _cmdBuff);

	void Display(VkCommandBuffer _cmdBuff, uint32_t _imageIndex);

	const VK_SwapChain* GetSwapChain();
	VkViewport* GetWiewport();
	VkRect2D* GetScissor();
};

#endif