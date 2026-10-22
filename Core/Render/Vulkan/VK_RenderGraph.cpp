#include "VK_RenderGraph.h"

#include "VK_Buffer.h"
#include "VK_Device.h"
#include "../../Logger.h"
#include "../../Asset/TextureConfig.h"

void VK_RenderGraph::AddResource(const std::string& _name, VkFormat _format, VkImageLayout _initialLayout,
                                 VkImageLayout _finalLayout, VkImageAspectFlags _imageAspect, VkImageUsageFlags _usage, const Vec2u& _size)
{
	VK_Resource resource;
	resource.name = _name;
	resource.initialLayout = _initialLayout;
	resource.finalLayout = _finalLayout;
	resource.image.Init(_format, _initialLayout, IMAGE_TYPE_2D, _imageAspect, _usage, 1, _size);

	m_resources[_name] = resource;
}

void VK_RenderGraph::AddPass(const std::string& _name, const std::vector<std::string>& _inputs,
                             const std::vector<std::string>& _outputs, std::function<void(IUniformManager*, const uint32_t&)> _updateFunc, std::
                             function<void(VkCommandBuffer&, IUniformManager*, const uint32_t&)> _executeFunc)
{
	VK_RenderPass pass;
	pass.name = _name;
	pass.inputs = _inputs;
	pass.outputs = _outputs;
	pass.executeFunc = _executeFunc;

	m_passes.emplace_back(pass);
}

void VK_RenderGraph::Compile()
{
	std::vector<std::vector<size_t>> dependencies(m_passes.size());
	std::vector<std::vector<size_t>> dependents(m_passes.size());

	std::unordered_map<std::string, size_t> resourceWriters;

	for (size_t i = 0; i < m_passes.size(); ++i)
	{
		for (size_t j = 0; j < m_passes[i].outputs.size(); ++j)
		{
			resourceWriters[m_passes[i].outputs[j]] = i;
		}
	}

	for (size_t i = 0; i < m_passes.size(); ++i)
	{
		for (size_t j = 0; j < m_passes[i].inputs.size(); ++j)
		{
			if (resourceWriters.contains(m_passes[i].inputs[j]))
			{
				dependencies[i].push_back(resourceWriters[m_passes[i].inputs[j]]);
				dependents[resourceWriters[m_passes[i].inputs[j]]].push_back(i);
			}
		}
	}

	std::vector<bool> visited(m_passes.size(), false);
	std::vector<bool> inStack(m_passes.size(), false);

	std::function<void(size_t)> visit = [&](size_t _node)
		{
			if (inStack[_node])
			{
				LOGGER_CRITICAL("Cycle detected in render graph ! ");
			}

			if (visited[_node])
			{
				return;
			}

			inStack[_node] = true;

			for (size_t i = 0; i < dependencies[_node].size(); ++i)
			{
				visit(dependencies[_node][i]);
			}

			inStack[_node] = false;
			visited[_node] = true;
			m_executionOrder.push_back(_node);
		};

	for (size_t i = 0; i < m_passes.size(); ++i)
	{
		if (!visited[i])
		{
			visit(i);
		}
	}
}

void VK_RenderGraph::Execute(VkCommandBuffer& _cmdBuff, VkQueue& _queue, IUniformManager* _uniformManager, const uint32_t& _frameIndex)
{

	for (size_t i = 0; i < m_executionOrder.size(); ++i)
	{
		for (size_t j = 0; j < m_passes[m_executionOrder[i]].inputs.size(); ++j)
		{
			VK_Resource& resource = m_resources[m_passes[m_executionOrder[i]].inputs[j]];

			resource.image.TransitionLayout(_cmdBuff, resource.initialLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		for (size_t j = 0; j < m_passes[m_executionOrder[i]].outputs.size(); ++j)
		{
			VK_Resource& resource = m_resources[m_passes[m_executionOrder[i]].outputs[j]];

			VkImageLayout newLayout = resource.image.GetImageAspect() == VK_IMAGE_ASPECT_COLOR_BIT ?
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			resource.image.TransitionLayout(_cmdBuff, resource.initialLayout, newLayout);
		}

		if (m_passes[m_executionOrder[i]].updateFunc)
		{
			m_passes[m_executionOrder[i]].updateFunc(_uniformManager, _frameIndex);
		}

		m_passes[m_executionOrder[i]].executeFunc(_cmdBuff, _uniformManager, _frameIndex);

		for (size_t j = 0; j < m_passes[m_executionOrder[i]].outputs.size(); ++j)
		{
			VK_Resource& resource = m_resources[m_passes[m_executionOrder[i]].outputs[j]];

			VkImageLayout oldLayout = resource.image.GetImageAspect() == VK_IMAGE_ASPECT_COLOR_BIT ?
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			resource.image.TransitionLayout(_cmdBuff, oldLayout, resource.finalLayout);
		}
	}
}

VK_Resource* VK_RenderGraph::GetResource(const std::string& _name)
{
	if (m_resources.contains(_name))
	{
		return &m_resources[_name];
	}

	return nullptr;
}
