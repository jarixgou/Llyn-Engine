#ifndef VK_UNIFORM_HANDLE__H
#define VK_UNIFORM_HANDLE__H
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "../../LlynCore.h"

#include "VK_Buffer.h"

struct VK_ShaderBuffer
{
	VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	std::vector<VK_Buffer> buffers;
	std::vector<void*> buffersMapped;

	uint32_t bufferSize = 0;

	std::vector<bool> createdFrame = {false, false};

	void CreateUniform(VK_Device* _device, const VK_LayoutInfo& _layoutInfo, void* _data);
	void StoreData(const VK_Device* _device, void* _data, size_t _size, VkDescriptorType _type, uint32_t _frameIndex);
	bool IsCreated(uint32_t _frameIndex) const;
};

#endif