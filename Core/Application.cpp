#include "Application.h"

#include <iostream>
#include <limits>
#include <map>

#include "Vertex.h"
#include "Utils/Utils.h"

const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f, 0.f}, {0.f, 0.f, 0.f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.f}, {0.f, 0.f, 0.f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.f}, {0.f, 0.f, 0.f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
};

const std::vector<char const*> validationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLyaers = false;
#else
constexpr bool enableValidationLyaers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

std::vector<const char*> GetRequiredInstanceExtensions();

static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT _severity,
	vk::DebugUtilsMessageTypeFlagsEXT _type,
	const vk::DebugUtilsMessengerCallbackDataEXT* _pCallbackData,
	void* _pUserData);

uint32_t ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& _surfaceCapabilities);
vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& _availableFormats);
vk::PresentModeKHR ChooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& _availablePresentModes);

static void FramebufferResizeCallback(GLFWwindow* _window, int _width, int _height);

void Application::Run()
{
	Init();
	MainLoop();
	Cleanup();
}

void Application::SetFrameBufferResized(bool _resized)
{
	m_frameBufferResized = _resized;
}

void Application::Init()
{
	InitWindow();
	InitVulkan();
}

void Application::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();

		DrawFrame();
	}

	m_device.waitIdle();
}

void Application::Cleanup()
{
	CleanupSwapChain();

	glfwDestroyWindow(m_window);

	glfwTerminate();
}

void Application::InitVulkan()
{
	CreateInstance();
	SetDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateGraphicsPipeline();
	CreateCommandPool();
	CreateVertexBuffer();
	CreateCommandBuffer();
	CreateSyncObjects();
}

void Application::CreateInstance()
{
	vk::ApplicationInfo appInfo{};
	appInfo.setPApplicationName("Llyn")
		.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
		.setPEngineName("Engine")
		.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
		.setApiVersion(VK_API_VERSION_1_4);

	// Get the required layers
	std::vector<char const*> requieredLayers;
	if (enableValidationLyaers)
	{
		requieredLayers.assign(validationLayers.begin(), validationLayers.end());
	}

	// Check if the required layer are supported by the Vulkan implementation
	auto layerProperties = m_context.enumerateInstanceLayerProperties();
	auto unsupportedLayerIt = std::ranges::find_if(requieredLayers, [&layerProperties](auto const& _requiredLayer)
		{
			return std::ranges::none_of(layerProperties, [_requiredLayer](auto const& _layerProperty)
				{
					return strcmp(_layerProperty.layerName, _requiredLayer) == 0;
				});
		});
	if (unsupportedLayerIt != requieredLayers.end())
	{
		std::cerr << "Required layer not supported : " + std::string(*unsupportedLayerIt) << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Get the required extensions
	auto requiredExtensions = GetRequiredInstanceExtensions();

	// Check if the required extensions are supported by the vulkan implementation
	auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
	auto unsupportedPropertyIt = std::ranges::find_if(requiredExtensions, [&extensionProperties](auto const& _requiredExtension)
		{
			return std::ranges::none_of(extensionProperties, [_requiredExtension](auto const& _extensionProperty)
				{
					return strcmp(_extensionProperty.extensionName, _requiredExtension) == 0;
				});
		});
	if (unsupportedPropertyIt != requiredExtensions.end())
	{
		std::cerr << "Required extension not supported : " + std::string(*unsupportedPropertyIt) << std::endl;
		std::exit(EXIT_FAILURE);
	}

	vk::InstanceCreateInfo createInfo{};
	createInfo.setPApplicationInfo(&appInfo)
		.setEnabledLayerCount(static_cast<uint32_t>(requieredLayers.size()))
		.setPpEnabledLayerNames(requieredLayers.data())
		.setEnabledExtensionCount(static_cast<uint32_t>(requiredExtensions.size()))
		.setPpEnabledExtensionNames(requiredExtensions.data());
	m_instance = vk::raii::Instance(m_context, createInfo);
}

void Application::SetDebugMessenger()
{
	if (!enableValidationLyaers)
	{
		return;
	}

	vk::DebugUtilsMessageSeverityFlagsEXT sverityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{};
	debugUtilsMessengerCreateInfoEXT.setMessageSeverity(sverityFlags)
		.setMessageType(messageTypeFlags)
		.setPfnUserCallback(&DebugCallBack);
}

void Application::CreateSurface()
{
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(*m_instance, m_window, nullptr, &surface) != 0)
	{
		std::cerr << "Failed to create window surface !" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	m_surface = vk::raii::SurfaceKHR(m_instance, surface);
}

void Application::PickPhysicalDevice()
{
	std::vector<vk::raii::PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
	auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& _physicalDevice)
		{
			return IsDeviceSuitable(_physicalDevice);
		});
	if (devIter == physicalDevices.end())
	{
		std::cerr << "Failed to find a suitable GPU" << std::endl;
	}
	m_physicalDevice = *devIter;
}

bool Application::IsDeviceSuitable(vk::raii::PhysicalDevice const& _physicalDevice)
{
	// Check if the physicalDevice supports the Vulkan 1.3 API version
	bool supportVulkan13 = _physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	// Check if any of the queue families support graphics operations
	auto queueFamilies = _physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& _qfp)
		{
			return !!(_qfp.queueFlags & vk::QueueFlagBits::eGraphics);
		});

	// Check if all required physicalDevice are available
	auto availableDeviceExtensions = _physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequieredExtensions = std::ranges::all_of(m_requiredDeviceExtension, [&availableDeviceExtensions](auto const& _requiredDeviceExtension)
		{
			return std::ranges::any_of(availableDeviceExtensions, [_requiredDeviceExtension](auto const& _availableDeviceExtension)
				{
					return strcmp(_availableDeviceExtension.extensionName, _requiredDeviceExtension) == 0;
				});
		});

	// Check if the physicalDevice supports the required features (dynamic rendering and extended dynamic state)
	auto features = _physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
	bool supportRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	// Return true if the physicalDevice meets all the criteria
	return supportVulkan13 && supportsGraphics && supportsAllRequieredExtensions && supportRequiredFeatures;
}

void Application::CreateLogicalDevice()
{
	// Find the index of the first queue family that supports graphics
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

	// Get the first index into queueFamilyProperties which supports both graphics and present
	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); ++qfpIndex)
	{
		if (((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics)) &&
			m_physicalDevice.getSurfaceSupportKHR(qfpIndex, *m_surface))
		{
			// Found a queue family that supports both graphics and present
			m_queueIndex = qfpIndex;
			qfpIndex = queueFamilyProperties.size();
		}
	}
	if (m_queueIndex == ~0)
	{
		std::cerr << "Could not find a queue for graphics and present -> terminating" << std::endl;
		std::exit(EXIT_FAILURE);
	}


	// Query for Vulkan 1.3 features
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain{};
	featureChain.get<vk::PhysicalDeviceVulkan13Features>().setDynamicRendering(true);
	featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().setExtendedDynamicState(true);

	// Create a device
	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{};
	deviceQueueCreateInfo.setQueueFamilyIndex(m_queueIndex);
	deviceQueueCreateInfo.setQueueCount(1);
	deviceQueueCreateInfo.setPQueuePriorities(&queuePriority);

	vk::DeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.setPNext(&featureChain.get<vk::PhysicalDeviceFeatures2>());
	deviceCreateInfo.setQueueCreateInfoCount(1);
	deviceCreateInfo.setPQueueCreateInfos(&deviceQueueCreateInfo);
	deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_requiredDeviceExtension.size()));
	deviceCreateInfo.setPpEnabledExtensionNames(m_requiredDeviceExtension.data());

	m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
	m_graphicsQueue = vk::raii::Queue(m_device, m_queueIndex, 0);

}

void Application::CreateSwapChain()
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
	m_swapChainExtent = ChooseSwapExtent(surfaceCapabilities);
	uint32_t minImageount = ChooseSwapMinImageCount(surfaceCapabilities);

	std::vector<vk::SurfaceFormatKHR> availableFormats = m_physicalDevice.getSurfaceFormatsKHR(*m_surface);
	m_swapChainSurfaceFormat = ChooseSwapSurfaceFormat(availableFormats);

	std::vector<vk::PresentModeKHR> availablePresentModes = m_physicalDevice.getSurfacePresentModesKHR(*m_surface);
	vk::PresentModeKHR presentMode = ChooseSwapPresentMode(availablePresentModes);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
	swapChainCreateInfo.setSurface(*m_surface);
	swapChainCreateInfo.setMinImageCount(minImageount);
	swapChainCreateInfo.setImageFormat(m_swapChainSurfaceFormat.format);
	swapChainCreateInfo.setImageColorSpace(m_swapChainSurfaceFormat.colorSpace);
	swapChainCreateInfo.setImageExtent(m_swapChainExtent);
	swapChainCreateInfo.setImageArrayLayers(1);
	swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	swapChainCreateInfo.setPreTransform(surfaceCapabilities.currentTransform);
	swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapChainCreateInfo.setPresentMode(presentMode);
	swapChainCreateInfo.setClipped(true);

	m_swapChain = vk::raii::SwapchainKHR(m_device, swapChainCreateInfo);
	m_swapChainImages = m_swapChain.getImages();
}

void Application::CleanupSwapChain()
{
	m_swapChainImageViews.clear();
	m_swapChain = nullptr;
}

void Application::RecreateSwapChain()
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(m_window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_window, &width, &height);
		glfwWaitEvents();
	}

	m_device.waitIdle();

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
}

vk::Extent2D Application::ChooseSwapExtent(vk::SurfaceCapabilitiesKHR const& _capabilities)
{
	if (_capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
	{
		return _capabilities.currentExtent;
	}

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(m_window, &width, &height);

	return {
		std::clamp<uint32_t>(width, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height)
	};
}

void Application::CreateImageViews()
{
	assert(m_swapChainImageViews.empty());

	vk::ImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
	imageViewCreateInfo.setFormat(m_swapChainSurfaceFormat.format);
	imageViewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	for (auto& image : m_swapChainImages)
	{
		imageViewCreateInfo.setImage(image);
		m_swapChainImageViews.emplace_back(m_device, imageViewCreateInfo);
	}
}

void Application::CreateGraphicsPipeline()
{
	vk::raii::ShaderModule shaderModule = CreateShaderModule(Utils::ReadFile("slang.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
	vertShaderStageInfo.setModule(shaderModule);
	vertShaderStageInfo.setPName("vertMain");

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
	fragShaderStageInfo.setModule(shaderModule);
	fragShaderStageInfo.setPName("fragMain");

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	auto bindingDescription = Vertex::GetBindingDescrition();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();
	vertexInputInfo.setVertexBindingDescriptionCount(1);
	vertexInputInfo.setPVertexBindingDescriptions(&bindingDescription);
	vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
	vertexInputInfo.setPVertexAttributeDescriptions(attributeDescriptions.data());

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

	vk::Viewport viewport{ 0.0f, 0.0f, static_cast<float>(m_swapChainExtent.width), static_cast<float>(m_swapChainExtent.height), 0.0, 1.0f };
	vk::Rect2D scissor{ vk::Offset2D{0, 0}, m_swapChainExtent };

	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicState{};

	dynamicState.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
	dynamicState.setPDynamicStates(dynamicStates.data());

	vk::PipelineViewportStateCreateInfo viewportState{};
	viewportState.setViewportCount(1);
	viewportState.setScissorCount(1);

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.setDepthClampEnable(vk::False);
	rasterizer.setRasterizerDiscardEnable(vk::False);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	rasterizer.setFrontFace(vk::FrontFace::eClockwise);
	rasterizer.setDepthBiasEnable(vk::False);
	rasterizer.setLineWidth(1.0f);

	vk::PipelineMultisampleStateCreateInfo multisampling{};
	multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	multisampling.setSampleShadingEnable(vk::False);

	vk::PipelineColorBlendAttachmentState colorBlendAttachement{};
	colorBlendAttachement.setBlendEnable(vk::False);
	colorBlendAttachement.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.setLogicOpEnable(vk::False);
	colorBlending.setLogicOp(vk::LogicOp::eCopy);
	colorBlending.setAttachmentCount(1);
	colorBlending.setPAttachments(&colorBlendAttachement);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.setSetLayoutCount(0);
	pipelineLayoutInfo.setPushConstantRangeCount(0);

	m_pipelineLayout = vk::raii::PipelineLayout(m_device, pipelineLayoutInfo);

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain{};
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setStageCount(2);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPStages(shaderStages);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPVertexInputState(&vertexInputInfo);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPInputAssemblyState(&inputAssembly);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPViewportState(&viewportState);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPRasterizationState(&rasterizer);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPMultisampleState(&multisampling);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPColorBlendState(&colorBlending);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setPDynamicState(&dynamicState);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setLayout(m_pipelineLayout);
	pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>().setRenderPass(nullptr);
	pipelineCreateInfoChain.get<vk::PipelineRenderingCreateInfo>().setColorAttachmentCount(1);
	pipelineCreateInfoChain.get<vk::PipelineRenderingCreateInfo>().setPColorAttachmentFormats(&m_swapChainSurfaceFormat.format);

	m_graphicsPipeline = vk::raii::Pipeline(m_device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}

void Application::CreateCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(m_queueIndex);

	m_commandPool = vk::raii::CommandPool(m_device, poolInfo);
}

void Application::CreateCommandBuffer()
{
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.setCommandPool(m_commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

	m_commandBuffers = vk::raii::CommandBuffers(m_device, allocInfo);
}

void Application::CreateSyncObjects()
{
	assert(m_presentCompleteSemaphores.empty() && m_renderFinishedSemaphores.empty() && m_inFlightFences.empty());

	for (size_t i = 0; i < m_swapChainImages.size(); ++i)
	{
		m_renderFinishedSemaphores.emplace_back(m_device, vk::SemaphoreCreateInfo());
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_presentCompleteSemaphores.emplace_back(vk::raii::Semaphore(m_device, vk::SemaphoreCreateInfo()));

		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
		m_inFlightFences.emplace_back(vk::raii::Fence(m_device, fenceInfo));
	}
}

void Application::RecordCommandBuffer(uint32_t _imageIndex)
{
	m_commandBuffers[m_frameIndex].begin({});

	// Before starting, transition the swapchain image to vk::ImageLayout::eColorAttachmentOptimal
	TransitionImageLayout(_imageIndex,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

	vk::RenderingAttachmentInfo attachmentInfo{};
	attachmentInfo.setImageView(m_swapChainImageViews[_imageIndex]);
	attachmentInfo.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
	attachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
	attachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
	attachmentInfo.setClearValue(clearColor);	

	vk::RenderingInfo renderingInfo{};
	vk::Rect2D area{};
	area.setOffset({ 0, 0 });
	area.setExtent(m_swapChainExtent);
	renderingInfo.setRenderArea(area);
	renderingInfo.setLayerCount(1);
	renderingInfo.setColorAttachmentCount(1);
	renderingInfo.setPColorAttachments(&attachmentInfo);

	m_commandBuffers[m_frameIndex].beginRendering(renderingInfo);
	m_commandBuffers[m_frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *m_graphicsPipeline);
	m_commandBuffers[m_frameIndex].bindVertexBuffers(0, *m_vertexBuffer, {0});
	m_commandBuffers[m_frameIndex].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_swapChainExtent.width), static_cast<float>(m_swapChainExtent.height), 0.0f, 1.0f));
	m_commandBuffers[m_frameIndex].setScissor(0, area);
	m_commandBuffers[m_frameIndex].draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);
	m_commandBuffers[m_frameIndex].endRendering();

	// After rendering, transition the swapchain image to vk::ImageLayout::ePresentSrcKHR
	TransitionImageLayout(_imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe);

	m_commandBuffers[m_frameIndex].end();
}

void Application::TransitionImageLayout(uint32_t _imageIndex, 
	vk::ImageLayout _oldLayout, 
	vk::ImageLayout _newLayout,
	vk::AccessFlags2 _srcAccessMask, 
	vk::AccessFlags2 _dstAccesMask, 
	vk::PipelineStageFlags2 _srcStageMask,
	vk::PipelineStageFlags2 _dstStageMask)
{
	vk::ImageMemoryBarrier2 barrier{};
	barrier.setSrcStageMask(_srcStageMask);
	barrier.setSrcAccessMask(_srcAccessMask);
	barrier.setDstStageMask(_dstStageMask);
	barrier.setDstAccessMask(_dstAccesMask);
	barrier.setOldLayout(_oldLayout);
	barrier.setNewLayout(_newLayout);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setImage(m_swapChainImages[_imageIndex]);

	vk::ImageSubresourceRange imageSubresource;
	imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
	imageSubresource.setBaseMipLevel(0);
	imageSubresource.setLevelCount(1);
	imageSubresource.setBaseArrayLayer(0);
	imageSubresource.setLayerCount(1);

	barrier.setSubresourceRange(imageSubresource);

	vk::DependencyInfo dependencyInfo{};
	dependencyInfo.setImageMemoryBarrierCount(1);
	dependencyInfo.setPImageMemoryBarriers(&barrier);

	m_commandBuffers[m_frameIndex].pipelineBarrier2(dependencyInfo);
}

void Application::CreateVertexBuffer()
{
	vk::BufferCreateInfo bufferInfo{};
	bufferInfo.setSize(sizeof(vertices[0]) * vertices.size());
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	m_vertexBuffer = vk::raii::Buffer(m_device, bufferInfo);

	vk::MemoryRequirements memRequirements = m_vertexBuffer.getMemoryRequirements();
	vk::MemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.setAllocationSize(memRequirements.size);
	memoryAllocateInfo.setMemoryTypeIndex(FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

	m_vertexBufferMemory = vk::raii::DeviceMemory(m_device, memoryAllocateInfo);
	m_vertexBuffer.bindMemory(*m_vertexBufferMemory, 0);

	void* data = m_vertexBufferMemory.mapMemory(0, bufferInfo.size);
	memcpy(data, vertices.data(), bufferInfo.size);
	m_vertexBufferMemory.unmapMemory();
}

void Application::DrawFrame()
{
	auto fenceResult = m_device.waitForFences(*m_inFlightFences[m_frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		std::cerr << "Failed to wait for fence" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	m_device.resetFences(*m_inFlightFences[m_frameIndex]);

	auto [result, imageIndex] = m_swapChain.acquireNextImage(UINT64_MAX, *m_presentCompleteSemaphores[m_frameIndex], nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		RecreateSwapChain();
		return;
	}
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		std::cerr << "Failed to acquire swap chain image !" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Only reset the fence if we are submitting work
	m_device.resetFences(*m_inFlightFences[m_frameIndex]);

	RecordCommandBuffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	vk::SubmitInfo submitInfo{};
	submitInfo.setWaitSemaphoreCount(1);
	submitInfo.setPWaitSemaphores(&*m_presentCompleteSemaphores[m_frameIndex]);
	submitInfo.setPWaitDstStageMask(&waitDestinationStageMask);
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&*m_commandBuffers[m_frameIndex]);
	submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setPSignalSemaphores(&*m_renderFinishedSemaphores[m_frameIndex]);

	m_graphicsQueue.submit(submitInfo, *m_inFlightFences[m_frameIndex]);

	vk::PresentInfoKHR presentInfoKHR{};
	presentInfoKHR.setWaitSemaphoreCount(1);
	presentInfoKHR.setPWaitSemaphores(&*m_renderFinishedSemaphores[m_frameIndex]);
	presentInfoKHR.setSwapchainCount(1);
	presentInfoKHR.setPSwapchains(&*m_swapChain);
	presentInfoKHR.setPImageIndices(&imageIndex);

	result = m_graphicsQueue.presentKHR(presentInfoKHR);
	if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
	{
		m_frameBufferResized = false;
		RecreateSwapChain();
	}
	else
	{
		assert(result == vk::Result::eSuccess);
	}

	m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

vk::raii::ShaderModule Application::CreateShaderModule(const std::vector<char>& _code)
{
	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.setCodeSize(_code.size() * sizeof(char));
	createInfo.setPCode(reinterpret_cast<const uint32_t*>(_code.data()));

	vk::raii::ShaderModule shaderModule{ m_device, createInfo };

	return shaderModule;
}

uint32_t Application::FindMemoryType(uint32_t _typeFilter, vk::MemoryPropertyFlags _properties) const
{
	vk::PhysicalDeviceMemoryProperties memProperties = m_physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (_typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
		{
			return i;
		}
	}

	std::cerr << "Failed to find suitable memory type!" << std::endl;
}

void Application::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(800, 600, "Llyn", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
}

std::vector<char const*> GetRequiredInstanceExtensions()
{
	uint32_t glfwExtensionCount = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLyaers)
	{
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	return extensions;
}

vk::Bool32 DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT _severity,
	vk::DebugUtilsMessageTypeFlagsEXT _type,
	const vk::DebugUtilsMessengerCallbackDataEXT* _pCallbackData,
	void* _pUserData)
{
	if (_severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		std::cerr << "Validation layer : type " << to_string(_type) << " msg : " << _pCallbackData->pMessage << std::endl;
	}

	return vk::False;
}

uint32_t ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& _surfaceCapabilities)
{
	auto minImageCount = (std::max)(3u, _surfaceCapabilities.minImageCount);
	if (0 < _surfaceCapabilities.maxImageCount && _surfaceCapabilities.maxImageCount < minImageCount)
	{
		minImageCount = _surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& _availableFormats)
{
	const auto formatIt = std::ranges::find_if(_availableFormats, [](const auto& _format)
		{
			return _format.format == vk::Format::eB8G8R8A8Srgb && _format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
		});

	return formatIt != _availableFormats.end() ? *formatIt : _availableFormats[0];
}

vk::PresentModeKHR ChooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& _availablePresentModes)
{
	assert(std::ranges::any_of(_availablePresentModes, [](auto _presentMode)
		{
			return _presentMode == vk::PresentModeKHR::eFifo;
		}));

	return std::ranges::any_of(_availablePresentModes, [](const vk::PresentModeKHR _value)
		{
			return vk::PresentModeKHR::eMailbox == _value;
		}) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

void FramebufferResizeCallback(GLFWwindow* _window, int _width, int _height)
{
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(_window));
	app->SetFrameBufferResized(true);
}