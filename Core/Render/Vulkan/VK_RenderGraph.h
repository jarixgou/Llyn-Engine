#ifndef VK_RENDER_GRAPH__H
#define VK_RENDER_GRAPH__H

#include <functional>
#include <string>
#include <vector>

#include "VK_Texture.h"

struct VK_Resource
{
	std::string name;
	VK_Image image;

	VkImageLayout initialLayout;
	VkImageLayout finalLayout;
};

struct VK_RenderPass
{
	std::string name = "Unknown";
	std::vector<std::string> inputs;
	std::vector<std::string> outputs;

	std::function<void(const uint32_t&)> updateFunc;
	std::function<void(VkCommandBuffer&)> executeFunc;
};


class VK_RenderGraph
{
private:
	std::unordered_map<std::string, VK_Resource> m_resources;
	std::vector<VK_RenderPass> m_passes;
	std::vector<size_t> m_executionOrder;

	std::vector<VkSemaphore> m_semaphores;
	std::vector<std::pair<size_t, size_t>> m_semaphoreSignalWaitPairs;
public:
	void AddResource(const std::string& _name, VkFormat _format, VkImageLayout _initialLayout, VkImageLayout _finalLayout, VkImageAspectFlags _imageAspect, VkImageUsageFlags _usage, const Vec2u& _size);
	void AddPass(const std::string& _name, const std::vector<std::string>& _inputs,
	             const std::vector<std::string>& _outputs, std::function<void(const uint32_t&)> _updateFunc, std::function<void(VkCommandBuffer&)> _executeFunc);

	void Compile();
	void Execute(VkCommandBuffer& _cmdBuff, VkQueue& _queue, const uint32_t& _frameIndex);

	VK_Resource* GetResource(const std::string& _name);
};

#endif