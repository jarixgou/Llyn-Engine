#ifndef APPLICATION__H
#define APPLICATION__H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
#include <vulkan/vulkan_raii.hpp>

class Application
{
private:
	GLFWwindow* m_window = nullptr;

	vk::raii::Context m_context;
	vk::raii::Instance m_instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
	vk::raii::SurfaceKHR m_surface = nullptr;

	vk::raii::PhysicalDevice m_physicalDevice = nullptr;
	vk::raii::Device m_device = nullptr;

	uint32_t m_queueIndex = ~0;

	vk::raii::Queue m_graphicsQueue = nullptr;

	vk::raii::SwapchainKHR m_swapChain = nullptr;
	std::vector<vk::Image> m_swapChainImages;
	std::vector<vk::raii::ImageView> m_swapChainImageViews;
	vk::SurfaceFormatKHR m_swapChainSurfaceFormat;
	vk::Extent2D m_swapChainExtent;

	vk::raii::PipelineLayout m_pipelineLayout = nullptr;
	vk::raii::Pipeline m_graphicsPipeline = nullptr;

	vk::raii::CommandPool m_commandPool = nullptr;
	std::vector<vk::raii::CommandBuffer> m_commandBuffers;

	std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores;
	std::vector<vk::raii::Fence> m_inFlightFences;

	uint32_t m_frameIndex = 0;

	bool m_frameBufferResized = false;

	vk::raii::Buffer m_vertexBuffer = nullptr;
	vk::raii::DeviceMemory m_vertexBufferMemory = nullptr;

	std::vector<const char*> m_requiredDeviceExtension = {
		vk::KHRSwapchainExtensionName };
public:
	void Run();

	void SetFrameBufferResized(bool _resized);
private:
	void Init();
	void MainLoop();
	void Cleanup();

	void InitVulkan();
	void CreateInstance();
	void SetDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	bool IsDeviceSuitable(vk::raii::PhysicalDevice const& _physicalDevice);
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CleanupSwapChain();
	void RecreateSwapChain();
	vk::Extent2D ChooseSwapExtent(vk::SurfaceCapabilitiesKHR const& _capabilities);
	void CreateImageViews();
	void CreateGraphicsPipeline();
	void CreateCommandPool();
	void CreateCommandBuffer();
	void CreateSyncObjects();
	void RecordCommandBuffer(uint32_t _imageIndex);
	void TransitionImageLayout(uint32_t _imageIndex,
		vk::ImageLayout _oldLayout,
		vk::ImageLayout _newLayout,
		vk::AccessFlags2 _srcAccessMask,
		vk::AccessFlags2 _dstAccesMask,
		vk::PipelineStageFlags2 _srcStageMask,
		vk::PipelineStageFlags2 _dstStageMask);

	void CreateVertexBuffer();

	void DrawFrame();

	[[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& _code);

	uint32_t FindMemoryType(uint32_t _typeFilter, vk::MemoryPropertyFlags _properties) const;

	void InitWindow();
};

#endif