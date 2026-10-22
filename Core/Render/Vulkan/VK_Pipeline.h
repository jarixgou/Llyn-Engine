#ifndef VK_PIPELINE__H
#define VK_PIPELINE__H

#include <vector>
#include <vulkan/vulkan.h>

#include "../../LlynCore.h"

class VK_Pipeline
{
private:
	std::vector<VK_DescriptorLayoutHandle> m_descriptorLayouts;

	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkPipeline m_pipeline = VK_NULL_HANDLE;

	void* m_pushConstantData = nullptr;
public:
	void Init(const VK_SwapChain* _swapChain, const PipelineInfo& _info);

	void Cleanup();

	const VkPipeline* GetPipeline() const;
	const VkPipelineLayout* GetPipelineLayout();
	
	std::vector<VK_DescriptorLayoutHandle> GetDescriptorLayouts();

	void SetPushConstant(void* _data);
	void* GetPushConstant();
private:
	void CreateDescriptorSetLayout(const VK_Device* _device, const SpirvReflectWrapper& _reflect);

	void CreateGraphicsPipeline(const VK_Device* _device, const VK_SwapChain* _swapChain, const PipelineInfo& _info, const SpirvReflectWrapper&
	                            _reflect);

	VkCullModeFlags GetCullMode(RenderFace _renderFace);

	static VkShaderModule CreateShaderModule(const VK_Device* _device, const std::vector<char>& _code);
	VkPipelineColorBlendAttachmentState CreateBlendMode(const PipelineInfo& _info);

	static VkDescriptorType GetDescriptorType(const UniformType& _uniformType);
	static VkShaderStageFlags GetShaderStageFlags(const ShaderStage& _shaderStage);
};

#endif