#include "VK_Pipeline.h"

#include "VK_Device.h"
#include "VK_DescriptorLayoutHandle.h"
#include "VK_LayoutInfo.h"
#include "PipelineInfo.h"
#include "VK_SwapChain.h"
#include "VK_DepthResources.h"
#include "../../Logger.h"
#include "../../Utils/Utils.h"
#include "../../Vertex.h"
#include "../../Utils/VK_Utils.h"

void VK_Pipeline::Init(const VK_Device* _device, const VK_SwapChain* _swapChain, const PipelineInfo& _info)
{
	LLYN_ASSERT(_device != nullptr && _swapChain != nullptr);

	CreateDescriptorSetLayout(_device);
	CreateGraphicsPipeline(_device, _swapChain, _info);
}

void VK_Pipeline::Cleanup(const VK_Device* _device)
{
	LLYN_ASSERT(_device != nullptr);

	LOGGER_INFO("Destroying VK_Pipeline");

	for (int i = 0; i < m_descriptorLayouts.size(); ++i)
	{
		vkDestroyDescriptorSetLayout(*_device->GetDevice(), m_descriptorLayouts[i].layout, VK_NULL_HANDLE);
	}
	LOGGER_INFO("VkDescriptorSetLayout is destroyed");

	vkDestroyPipelineLayout(*_device->GetDevice(), m_pipelineLayout, nullptr);
	LOGGER_INFO("VkPipelineLayout is destroyed");

	vkDestroyPipeline(*_device->GetDevice(), m_pipeline, nullptr);
	LOGGER_INFO("VkPipeline is destroyed");
}

const VkPipeline* VK_Pipeline::GetPipeline() const
{
	return &m_pipeline;
}

const VkPipelineLayout* VK_Pipeline::GetPipelineLayout()
{
	return &m_pipelineLayout;
}

std::vector<VK_DescriptorLayoutHandle> VK_Pipeline::GetDescriptorLayouts()
{
	return m_descriptorLayouts;
}

void VK_Pipeline::CreateDescriptorSetLayout(const VK_Device* _device)
{
	std::vector<VK_LayoutInfo> bindlessLayoutInfo =
	{
		{"textures", 0, 0, 1024,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, true }
	};
	VK_DescriptorLayoutHandle bindlessDescriptor;
	bindlessDescriptor.CreateLayout(_device, bindlessLayoutInfo);
	m_descriptorLayouts.emplace_back(bindlessDescriptor);

	std::vector<VK_LayoutInfo> coreLayoutInfo =
	{
		{"vertices", 0, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, false},
		{"indices", 1, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, false},
		{"materials", 2, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, false}
	};
	VK_DescriptorLayoutHandle coreDescriptor;
	coreDescriptor.CreateLayout(_device, coreLayoutInfo);
	m_descriptorLayouts.emplace_back(coreDescriptor);

	std::vector<VK_LayoutInfo> sceneLayoutInfo =
	{
		{"matrices", 0, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, false},
		{"metaDatas", 1, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, false}
	};
	VK_DescriptorLayoutHandle sceneDescriptor;
	sceneDescriptor.CreateLayout(_device, sceneLayoutInfo);
	m_descriptorLayouts.emplace_back(sceneDescriptor);

	std::vector<VK_LayoutInfo> lightLayoutInfo =
	{
		{"dirLights", 0, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, false},
		{"pointLights", 1, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, false},
		{"spotLights", 2, 0, 1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, false},
		{"lightData", 3, 0, 1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, false}
	};
	VK_DescriptorLayoutHandle lightDescriptor;
	lightDescriptor.CreateLayout(_device, lightLayoutInfo);
	m_descriptorLayouts.emplace_back(lightDescriptor);
}

void VK_Pipeline::CreateGraphicsPipeline(const VK_Device* _device, const VK_SwapChain* _swapChain, const PipelineInfo& _info)
{
	LLYN_ASSERT(_device != nullptr && _swapChain != nullptr);

	VkShaderModule shaderModule = CreateShaderModule(_device, Utils::ReadFile("Core/Shaders/slang.spv"));

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.pNext = VK_NULL_HANDLE;
	vertShaderStageInfo.pSpecializationInfo = VK_NULL_HANDLE;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = shaderModule;
	vertShaderStageInfo.pName = "vertMain";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.pNext = VK_NULL_HANDLE;
	fragShaderStageInfo.pSpecializationInfo = VK_NULL_HANDLE;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = shaderModule;
	fragShaderStageInfo.pName = "fragMain";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// If I want to enable the direct drawing
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = VK_NULL_HANDLE;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = VK_NULL_HANDLE;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = VK_NULL_HANDLE;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.pNext = VK_NULL_HANDLE;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	const VkExtent2D* extent = _swapChain->GetExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent->width);
	viewport.height = static_cast<float> (extent->height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = *extent;

	const std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportCreateInfo{};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pScissors = &scissor;
	viewportCreateInfo.scissorCount = 1;

	VkCullModeFlags cullMode = GetCullMode(_info.renderFace);

	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.pNext = VK_NULL_HANDLE;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.cullMode = cullMode;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.pNext = VK_NULL_HANDLE;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleCreateInfo.sampleShadingEnable = VK_TRUE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = CreateBlendMode(_info);
	VkPipelineColorBlendStateCreateInfo colorBlendfCreateInfo{};
	colorBlendfCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendfCreateInfo.pNext = VK_NULL_HANDLE;
	colorBlendfCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendfCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendfCreateInfo.attachmentCount = 1;
	colorBlendfCreateInfo.pAttachments = &colorBlendAttachment;

	uint32_t lightingModeValue = 3;

	VkSpecializationMapEntry specMapEntry{};
	specMapEntry.constantID = 0;
	specMapEntry.offset = 0;
	specMapEntry.size = sizeof(uint32_t);

	VkSpecializationInfo specInfo{};
	specInfo.mapEntryCount = 1;
	specInfo.pMapEntries = &specMapEntry;
	specInfo.dataSize = sizeof(uint32_t);
	specInfo.pData = &lightingModeValue;

	shaderStages[1].pSpecializationInfo = &specInfo;

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(Vec3f);

	std::vector<VkDescriptorSetLayout> setLayouts(4);
	for (int i = 0; i < 4; ++i)
	{
		setLayouts[i] = m_descriptorLayouts[i].layout;
	}

	VkPipelineLayoutCreateInfo pipeLayoutCreateInfo{};
	pipeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeLayoutCreateInfo.pNext = VK_NULL_HANDLE;
	pipeLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	pipeLayoutCreateInfo.pSetLayouts = setLayouts.data();
	pipeLayoutCreateInfo.pushConstantRangeCount = 1;
	pipeLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

	VK_CHECK(vkCreatePipelineLayout(*_device->GetDevice(), &pipeLayoutCreateInfo, nullptr, &m_pipelineLayout), "Failed to create Pipeline Layout");

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.pNext = VK_NULL_HANDLE;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

	const VkFormat surfaceFormat = _swapChain->GetSwapChainSurface()->format;

	VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
	pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	pipelineRenderingCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipelineRenderingCreateInfo.pColorAttachmentFormats = &surfaceFormat;
	pipelineRenderingCreateInfo.depthAttachmentFormat = VK_DepthResources::FinDepthFormat(_device);

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = shaderStages;
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewportCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &colorBlendfCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout = m_pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = nullptr;

	VK_CHECK(vkCreateGraphicsPipelines(*_device->GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, VK_NULL_HANDLE, &m_pipeline),
		"Failed to create Graphics Pipeline !");
}

VkCullModeFlags VK_Pipeline::GetCullMode(RenderFace _renderFace)
{
	switch (_renderFace)
	{
	case RenderFace::BACK:
		return VK_CULL_MODE_BACK_BIT;
		break;
	case RenderFace::FRONT:
		return VK_CULL_MODE_FRONT_BIT;
		break;
	case RenderFace::BOTH:
		return VK_CULL_MODE_FRONT_AND_BACK;
		break;
	default:;
	}

	return VK_CULL_MODE_BACK_BIT;
}

VkShaderModule VK_Pipeline::CreateShaderModule(const VK_Device* _device, const std::vector<char>& _code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

	VkShaderModule shaderModule = VK_NULL_HANDLE;
	VK_CHECK(vkCreateShaderModule(*_device->GetDevice(), &createInfo, nullptr, &shaderModule), "Failed to create Shader Module");

	return shaderModule;
}

VkPipelineColorBlendAttachmentState VK_Pipeline::CreateBlendMode(const PipelineInfo& _info)
{
	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	if (_info.surfaceType == SurfaceType::OPAQUE)
	{
		blendAttachment.blendEnable = VK_FALSE;
	}
	else
	{
		blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		switch (_info.blendMode) {
		case BlendMode::ALPHA:
			blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;
		case BlendMode::PRE_MULTIPLY:
			blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			break;
		case BlendMode::ADDITIVE:
			blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			break;
		case BlendMode::MULTIPLY:
			blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
			blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			break;
		}
	}

	return blendAttachment;
}
