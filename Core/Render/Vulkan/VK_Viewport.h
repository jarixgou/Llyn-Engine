#ifndef VK_VIEWPORT__H
#define VK_VIEWPORT__H

#include "VK_DescriptorHandle.h"
#include "../../LlynCore.h"

class VK_Viewport
{
private:
	VK_Resource* m_actualResource = nullptr;
	VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
public:
	void Init(const std::string& _defaultResource, VK_RenderGraph* _renderGraph);
	void Update(VK_RenderGraph* _renderGraph);
	void Cleanup();
};

#endif