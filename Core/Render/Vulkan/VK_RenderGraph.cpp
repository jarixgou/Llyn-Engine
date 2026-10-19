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

	for (size_t i = 0; i < m_passes.size(); ++i)
	{
		for (size_t j = 0; j < dependencies[i].size(); ++j)
		{
			VkSemaphoreCreateInfo semaphoreInfo;
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreInfo.pNext = VK_NULL_HANDLE;
			semaphoreInfo.flags = 0;

			VkSemaphore semaphore;
			VK_CHECK(vkCreateSemaphore(*VK_Device::Get().GetDevice(), &semaphoreInfo, VK_NULL_HANDLE, &semaphore),
				"Failed to create semaphore for render graph ! ");

			m_semaphores.emplace_back(semaphore);
			m_semaphoreSignalWaitPairs.emplace_back(dependencies[i][j], i);
		}
	}
}

void VK_RenderGraph::Execute(VkCommandBuffer& _cmdBuff, VkQueue& _queue, IUniformManager* _uniformManager, const uint32_t& _frameIndex)
{
	std::vector<VkSemaphore> waitSemaphores;
	std::vector<VkPipelineStageFlags> waitStages;
	std::vector<VkSemaphore> signalSemaphores;

	for (size_t i = 0; i < m_executionOrder.size(); ++i)
	{
		waitSemaphores.clear();
		waitStages.clear();

		for (size_t j = 0; j < m_semaphoreSignalWaitPairs.size(); ++j)
		{
			if (m_semaphoreSignalWaitPairs[j].second == m_executionOrder[i])
			{
				waitSemaphores.emplace_back(m_semaphores[j]);
				waitStages.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
			}
		}

		signalSemaphores.clear();
		for (size_t j = 0; j < m_semaphoreSignalWaitPairs.size(); ++j)
		{
			if (m_semaphoreSignalWaitPairs[j].first == m_executionOrder[i])
			{
				signalSemaphores.emplace_back(m_semaphores[j]);
			}
		}

		VK_Buffer::BeginCommandBuffer(_cmdBuff, 0);

		for (size_t j = 0; j < m_passes[i].inputs.size(); ++j)
		{
			VK_Resource& resource = m_resources[m_passes[m_executionOrder[i]].inputs[j]];

			resource.image.TransitionLayout(_cmdBuff, resource.initialLayout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		for (size_t j = 0; j < m_passes[i].outputs.size(); ++j)
		{
			VK_Resource& resource = m_resources[m_passes[m_executionOrder[i]].outputs[j]];

			resource.image.TransitionLayout(_cmdBuff, resource.initialLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		if (m_passes[m_executionOrder[i]].updateFunc)
		{
			m_passes[m_executionOrder[i]].updateFunc(_uniformManager, _frameIndex);
		}

		m_passes[m_executionOrder[i]].executeFunc(_cmdBuff, _uniformManager, _frameIndex);

		for (size_t j = 0; j < m_passes[i].outputs.size(); ++j)
		{
			VK_Resource& resource = m_resources[m_passes[m_executionOrder[i]].outputs[j]];

			resource.image.TransitionLayout(_cmdBuff, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, resource.finalLayout);
		}

		vkEndCommandBuffer(_cmdBuff);

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_cmdBuff;
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
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
