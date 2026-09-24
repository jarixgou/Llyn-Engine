#ifndef VK_BUFFER__H
#define VK_BUFFER__H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "../../Vector/FwdVec2.h"
#include "../../LlynCore.h"

struct VK_Buffer
{
	static VK_Device* device;
	static VK_FrameSync* frameSync;

	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	void* data;

	static void SetDevice(VK_Device* _device);

	static void SetFrameSync(VK_FrameSync* _frameSync);

	void Cleanup();

	static VkCommandPool CreateCommandPool();

	static std::vector<VkCommandBuffer> CreateCommandBuffer(VkCommandPool _cmdPool, uint32_t _bufferCount);

	static VK_Buffer CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage,
	                              VkMemoryPropertyFlags _memProps);

	static VK_Buffer CreateStagingBuffer(void* _data, VkDeviceSize _size, VkBufferUsageFlags _usage);

	static VK_Buffer CreateIndirectBuffer(VkDeviceSize _size);
	static VK_Buffer CreateUBO(VkDeviceSize _size);
	static VK_Buffer CreateSSBO(void* _data, VkDeviceSize _size);

	static void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size);
	static void CopyBufferToImage(VkCommandBuffer _cmdBuffer, const VK_Buffer* _buffer, VkImage* _image, Vec2<uint32_t> _imageSize);

	static VkCommandBuffer BeginSingleTimeCommand();
	static void EndSingleTimeCommand(VkCommandBuffer _cmdBuffer);

	static void BeginCommandBuffer(VkCommandBuffer _cmdBuffer, VkCommandBufferUsageFlags _usage);
};

#endif