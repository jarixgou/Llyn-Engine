#include "VK_Pipeline.h"

#include <filesystem>
#include <JSON/simdjson.h>

#include "VK_Device.h"
#include "VK_DescriptorLayoutHandle.h"
#include "VK_LayoutInfo.h"
#include "PipelineInfo.h"
#include "VK_SwapChain.h"
#include "VK_DepthResources.h"
#include "VK_Texture.h"
#include "../SpirvReflectWrapper.h"
#include "../../Logger.h"
#include "../../Utils/Utils.h"
#include "../../Vertex.h"

#include "../UniformType.h"
#include "../ShaderStage.h"

void VK_Pipeline::Init(const VK_SwapChain* _swapChain, const PipelineInfo& _info)
{
	LLYN_ASSERT(_swapChain != nullptr);

	SpirvReflectWrapper reflect;
	reflect.LoadShader(_info.shaderPath);

	CreateDescriptorSetLayout(&VK_Device::Get(), reflect);
	CreateGraphicsPipeline(&VK_Device::Get(), _swapChain, _info, reflect);
}

void VK_Pipeline::Cleanup()
{
	LOGGER_INFO("Destroying VK_Pipeline");

	for (int i = 0; i < m_descriptorLayouts.size(); ++i)
	{
		vkDestroyDescriptorSetLayout(*VK_Device::Get().GetDevice(), m_descriptorLayouts[i].layout, VK_NULL_HANDLE);
	}
	LOGGER_INFO("VkDescriptorSetLayout is destroyed");

	vkDestroyPipelineLayout(*VK_Device::Get().GetDevice(), m_pipelineLayout, nullptr);
	LOGGER_INFO("VkPipelineLayout is destroyed");

	vkDestroyPipeline(*VK_Device::Get().GetDevice(), m_pipeline, nullptr);
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

void VK_Pipeline::CreateDescriptorSetLayout(const VK_Device* _device, const SpirvReflectWrapper& _reflect)
{
	m_descriptorLayouts.resize(_reflect.m_setInfos.size());
	for (size_t i = 0; i < _reflect.m_setInfos.size(); ++i)
	{
		std::vector<VK_LayoutInfo> layoutInfos(_reflect.m_setInfos[i].shaderInfos.size());
		for (size_t j = 0; j < _reflect.m_setInfos[i].shaderInfos.size(); ++j)
		{
			layoutInfos[j].name = _reflect.m_setInfos[i].shaderInfos[j].name;
			layoutInfos[j].binding = _reflect.m_setInfos[i].shaderInfos[j].binding;
			layoutInfos[j].offset = 0;
			layoutInfos[j].count = _reflect.m_setInfos[i].shaderInfos[j].bindless ? BINDLESS_COUNT : 1;
			layoutInfos[j].type = GetDescriptorType(_reflect.m_setInfos[i].shaderInfos[j].uniformType);
			layoutInfos[j].stageFlags = GetShaderStageFlags(_reflect.m_setInfos[i].shaderInfos[j].shaderStage);
			layoutInfos[j].bindless = _reflect.m_setInfos[i].shaderInfos[j].bindless;
		}
		VK_DescriptorLayoutHandle layoutHandle;
		layoutHandle.CreateLayout(_device, layoutInfos);
		m_descriptorLayouts[i] = layoutHandle;
	}
}

void VK_Pipeline::CreateGraphicsPipeline(const VK_Device* _device, const VK_SwapChain* _swapChain, const PipelineInfo& _info, const SpirvReflectWrapper&
                                         _reflect)
{
	LLYN_ASSERT(_device != nullptr && _swapChain != nullptr);

	VkShaderModule shaderModule = CreateShaderModule(_device, Utils::ReadFile(_info.shaderPath));

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
	std::vector<VkVertexInputAttributeDescription> vertexAttribute(_reflect.m_inputAttribute.size());
	for (size_t i = 0; i < _reflect.m_inputAttribute.size(); ++i)
	{
		vertexAttribute[i].binding = _reflect.m_inputAttribute[i].binding;
		vertexAttribute[i].location = _reflect.m_inputAttribute[i].location;
		vertexAttribute[i].format = Utils::GetVkFormat(_reflect.m_inputAttribute[i].format);
		vertexAttribute[i].offset = _reflect.m_inputAttribute[i].offset;
	}

	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = _reflect.m_attributeInputSizeByte;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = VK_NULL_HANDLE;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttribute.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = vertexAttribute.data();

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

	std::vector<VkPushConstantRange> pushConstants(_reflect.m_pushConstants.size());
	for (size_t i = 0; i < _reflect.m_pushConstants.size(); ++i)
	{
		pushConstants[i].stageFlags = GetShaderStageFlags(_reflect.m_pushConstants[i].shaderStage);
		pushConstants[i].offset = _reflect.m_pushConstants[i].offset;
		pushConstants[i].size = _reflect.m_pushConstants[i].size;
	}

	std::vector<VkDescriptorSetLayout> setLayouts(m_descriptorLayouts.size());
	for (int i = 0; i < m_descriptorLayouts.size(); ++i)
	{
		setLayouts[i] = m_descriptorLayouts[i].layout;
	}

	VkPipelineLayoutCreateInfo pipeLayoutCreateInfo{};
	pipeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeLayoutCreateInfo.pNext = VK_NULL_HANDLE;
	pipeLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
	pipeLayoutCreateInfo.pSetLayouts = setLayouts.data();
	pipeLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
	pipeLayoutCreateInfo.pPushConstantRanges = pushConstants.data();

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

	if (_info.imageFormats.size() != _reflect.m_imageOutputCount)
	{
		LOGGER_CRITICAL("The formats you passed are not the same on the shader ! ");
	}

	std::vector<VkFormat> formats(_reflect.m_imageOutputCount);
	for (int i = 0; i < _reflect.m_imageOutputCount; ++i)
	{
		formats[i] = VK_Texture::GetFormat(_info.imageFormats[i]);
	}

	VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
	pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	pipelineRenderingCreateInfo.pNext = VK_NULL_HANDLE;
	pipelineRenderingCreateInfo.colorAttachmentCount = static_cast<uint32_t>(formats.size());
	pipelineRenderingCreateInfo.pColorAttachmentFormats = formats.data();
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

VkDescriptorType VK_Pipeline::GetDescriptorType(const UniformType& _uniformType)
{
	switch (_uniformType)
	{
	case UNIFORM_TYPE_CONSTANT:					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case UNIFORM_TYPE_STORAGE:					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case UNIFORM_TYPE_COMBINED_IMAGE_SAMPLER:	return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case UNIFORM_TYPE_SAMPLED_IMAGE:			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case UNIFORM_TYPE_MAX:
		LOGGER_ERROR("Invalid uniform type ! ");
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

	LOGGER_ERROR("Unsupported uniform type ! ");
	return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

VkShaderStageFlags VK_Pipeline::GetShaderStageFlags(const ShaderStage& _shaderStage)
{
	switch (_shaderStage)
	{
	case SHADER_STAGE_VERTEX:		return VK_SHADER_STAGE_VERTEX_BIT;
	case SHADER_STAGE_FRAGMENT:		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case SHADER_STAGE_COMPUTE:		return VK_SHADER_STAGE_COMPUTE_BIT;
	case SHADER_STAGE_MAX:
		LOGGER_ERROR("Invalid shader stage !");
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	LOGGER_ERROR("Unsupported shader stage ! ");
	return VK_SHADER_STAGE_VERTEX_BIT;
}
