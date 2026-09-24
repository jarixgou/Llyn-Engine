#include "VK_ImGui.h"

#include "VK_Device.h"
#include "VK_SwapChain.h"

#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_impl_glfw.h"
#include "../../ImGui/imgui_impl_vulkan.h"

#include "VK_RenderWindow.h"
#include "VK_Buffer.h"
#include "VK_DepthResources.h"
#include "VK_FrameSync.h"
#include "VK_Instance.h"
#include "../Window.h"

#ifdef NDEBUG
constexpr bool enableGLFWCallBack = true;
#else
constexpr bool enableGLFWCallBack = false;
#endif

static void VkCheckImGui(VkResult _err)
{
	if (_err == 0)
	{
		return;
	}

	fprintf(stderr, "[ImGui Vulkan] Error: %d\n", _err);

	if (_err < 0)
	{
		abort();
	}
}

void VK_ImGui::Init(const VK_Instance* _instance, const VK_Device* _device, const VK_SwapChain* _swapChain, const VK_FrameSync* _frameSync)
{
	const VkExtent2D* extent = _swapChain->GetExtent();
	m_frameBufferSize = { extent->width, extent->height };

	CreateDescriptorPool(_device);

	InitImGui(_instance, _device, _swapChain, _frameSync);
}

void VK_ImGui::Cleanup(const VK_Device* _device, VkCommandPool _cmdPool)
{
	vkFreeCommandBuffers(*_device->GetDevice(), _cmdPool, static_cast<uint32_t>(m_cmdBuffs.size()), m_cmdBuffs.data());

	ImGui_ImplVulkan_Shutdown();

	vkDestroyDescriptorPool(*_device->GetDevice(), m_desscriptorPool, VK_NULL_HANDLE);

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

VkCommandBuffer VK_ImGui::Draw(VK_RenderWindow* _renderWindow, uint32_t _frameIndex, uint32_t _imageIndex)
{
	VK_Buffer::BeginCommandBuffer(m_cmdBuffs[_frameIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	_renderWindow->BeginRendering(m_cmdBuffs[_frameIndex], _imageIndex, nullptr, nullptr);

	ImDrawData* drawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(drawData, m_cmdBuffs[_frameIndex]);

	_renderWindow->EndRendering(m_cmdBuffs[_frameIndex]);

	return m_cmdBuffs[_frameIndex];
}

void VK_ImGui::InitImGui(const VK_Instance* _instance, const VK_Device* _device, const VK_SwapChain* _swapChain, const VK_FrameSync* _frameSync)
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	//ImGui::GetStyle().FontScaleMain = 1.5f;
	ImGui_ImplGlfw_InitForVulkan(Window::Get().GetGLFWWindow(), enableGLFWCallBack);
	
	const VkFormat swapChainFormat = _swapChain->GetFormat();
	const VkFormat depthFormat = VK_DepthResources::FinDepthFormat(_device);

	VkPipelineRenderingCreateInfo pipelineRenderingInfo{};
	pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	pipelineRenderingInfo.pNext = VK_NULL_HANDLE;
	pipelineRenderingInfo.colorAttachmentCount = 1;
	pipelineRenderingInfo.pColorAttachmentFormats = &swapChainFormat;
	pipelineRenderingInfo.depthAttachmentFormat = depthFormat;
	pipelineRenderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.ApiVersion = _instance->GetAPIVersion();
	initInfo.Instance = *_instance->GetInstance();
	initInfo.PhysicalDevice = *_device->GetPhycicalDevice();
	initInfo.Device = *_device->GetDevice();
	initInfo.QueueFamily = _device->GetQueueIndex();
	initInfo.Queue = *_device->GetGraphicsQueue();
	initInfo.DescriptorPool = m_desscriptorPool;
	initInfo.MinImageCount = _swapChain->GetMinImageCount();
	initInfo.ImageCount = static_cast<uint32_t>(_swapChain->GetImageCount());
	initInfo.PipelineCache = VK_NULL_HANDLE;
	initInfo.PipelineInfoMain = { VK_NULL_HANDLE, 0, VK_SAMPLE_COUNT_1_BIT, {}, pipelineRenderingInfo };
	initInfo.UseDynamicRendering = true;
	initInfo.Allocator = nullptr;
	initInfo.CheckVkResultFn = VkCheckImGui;

	ImGui_ImplVulkan_Init(&initInfo);

	m_cmdBuffs = VK_Buffer::CreateCommandBuffer(*_frameSync->GetCommandPool(), MAX_FRAMES_IN_FLIGHT);
}

void VK_ImGui::CreateDescriptorPool(const VK_Device* _device)
{
	VkDescriptorPoolSize poolSizes[] =
	{
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
	};

	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = VK_NULL_HANDLE;
	poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolCreateInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
	poolCreateInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
	poolCreateInfo.pPoolSizes = poolSizes;

	VK_CHECK(vkCreateDescriptorPool(*_device->GetDevice(), &poolCreateInfo, VK_NULL_HANDLE, &m_desscriptorPool),
		"Failed to create descriptor pool for ImGui !");
}