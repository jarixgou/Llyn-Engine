#include "VK_Buffer.h"

#include "VK_Device.h"
#include "VK_FrameSync.h"
#include "../../Utils/Vulkan/VK_MemoryHelper.h"
#include "../../Vector/Vec2.h"

VK_Device* VK_Buffer::device = nullptr;
VK_FrameSync* VK_Buffer::frameSync = nullptr;

void VK_Buffer::SetDevice(VK_Device* _device)
{
	LLYN_ASSERT(_device != nullptr);

	device = _device;
}

void VK_Buffer::SetFrameSync(VK_FrameSync* _frameSync)
{
	LLYN_ASSERT(_frameSync != nullptr);

	frameSync = _frameSync;
}

void VK_Buffer::Cleanup()
{
	vkDestroyBuffer(*device->GetDevice(), buffer, VK_NULL_HANDLE);
	vkFreeMemory(*device->GetDevice(), memory, VK_NULL_HANDLE);
}

VkCommandPool VK_Buffer::CreateCommandPool()
{
	LLYN_ASSERT(device != nullptr);

	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.pNext = VK_NULL_HANDLE;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = device->GetQueueIndex();

	VkCommandPool cmdPool = VK_NULL_HANDLE;
	VK_CHECK(vkCreateCommandPool(*device->GetDevice(), &cmdPoolCreateInfo, VK_NULL_HANDLE, &cmdPool),
		"Failed to create command pool");

	return cmdPool;
}

std::vector<VkCommandBuffer> VK_Buffer::CreateCommandBuffer(VkCommandPool _cmdPool,
	uint32_t _bufferCount)
{
	LLYN_ASSERT(device != nullptr);

	VkCommandBufferAllocateInfo cmdBuffAllocInfo{};
	cmdBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBuffAllocInfo.pNext = VK_NULL_HANDLE;
	cmdBuffAllocInfo.commandPool = _cmdPool;
	cmdBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBuffAllocInfo.commandBufferCount = _bufferCount;

	std::vector<VkCommandBuffer> cmdBuffs(_bufferCount);
	VK_CHECK(vkAllocateCommandBuffers(*device->GetDevice(), &cmdBuffAllocInfo, cmdBuffs.data()),
		"Failed to allocate command buffers !");

	return cmdBuffs;
}

VK_Buffer VK_Buffer::CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage,
	VkMemoryPropertyFlags _memProps)
{
	LLYN_ASSERT(device != nullptr);

	VK_Buffer buffer{};

	VkBufferCreateInfo buffCreateInfo{};
	buffCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffCreateInfo.pNext = VK_NULL_HANDLE;
	buffCreateInfo.size = _size;
	buffCreateInfo.usage = _usage;
	buffCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(*device->GetDevice(), &buffCreateInfo, VK_NULL_HANDLE, &buffer.buffer),
		"Failed to create buffer !");

	buffer.memory = VK_MemoryHelper::AllocateDeviceMemory(buffer.buffer, _memProps);

	return buffer;
}

VK_Buffer VK_Buffer::CreateStagingBuffer(void* _data, VkDeviceSize _size, VkBufferUsageFlags _usage)
{
	LLYN_ASSERT(device != nullptr);

	// Creating staging buffer 
	VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	VK_Buffer stagingBuff = CreateBuffer(_size, usage, memProps);

	// Map the memory of the stage buffer
	void* mem;
	const VkDeviceSize offset = 0;
	const VkMemoryMapFlags flags = 0;
	VK_CHECK(vkMapMemory(*device->GetDevice(), stagingBuff.memory, offset, _size, flags, &mem),
		"Failed to map memory !");

	// Copy buffer
	memcpy(mem, _data, _size);

	// Unmap the mapped memory
	vkUnmapMemory(*device->GetDevice(), stagingBuff.memory);

	// Create the final buffer
	usage = _usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VK_Buffer finalBuffer = CreateBuffer(_size, usage, memProps);

	// Copy the staging buffer to the final buffer
	CopyBuffer(stagingBuff.buffer, finalBuffer.buffer, _size);

	stagingBuff.Cleanup();

	return finalBuffer;
}

VK_Buffer VK_Buffer::CreateIndirectBuffer(VkDeviceSize _size)
{
	LLYN_ASSERT(device != nullptr);

	VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	VK_Buffer indirectBuffer = CreateBuffer(_size, usage, memProps);

	VK_CHECK(vkMapMemory(*device->GetDevice(), indirectBuffer.memory, 0, _size, 0, &indirectBuffer.data),
		"Failed to map indirect buffer memory");

	return indirectBuffer;
}

VK_Buffer VK_Buffer::CreateUBO(VkDeviceSize _size)
{
	LLYN_ASSERT(device != nullptr);

	return CreateBuffer(_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

VK_Buffer VK_Buffer::CreateSSBO(void* _data, VkDeviceSize _size)
{
	return CreateStagingBuffer(_data, _size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

void VK_Buffer::CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size)
{
	VkCommandBuffer cmdBuffer = BeginSingleTimeCommand();

	VkBufferCopy buffCopy{};
	buffCopy.size = _size;
	buffCopy.dstOffset = 0;
	buffCopy.srcOffset = 0;

	vkCmdCopyBuffer(cmdBuffer, _srcBuffer, _dstBuffer, 1, &buffCopy);

	EndSingleTimeCommand(cmdBuffer);
}

void VK_Buffer::CopyBufferToImage(VkCommandBuffer _cmdBuffer, const VK_Buffer* _buffer, VkImage* _image, Vec2<uint32_t> _imageSize)
{
	VkImageSubresourceLayers imgSubRessources{};
	imgSubRessources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgSubRessources.mipLevel = 0;
	imgSubRessources.baseArrayLayer = 0;
	imgSubRessources.layerCount = 1;

	VkBufferImageCopy buffImgCpy{};
	buffImgCpy.bufferOffset = 0;
	buffImgCpy.bufferRowLength = 0;
	buffImgCpy.bufferImageHeight = 0;
	buffImgCpy.imageSubresource = imgSubRessources;
	buffImgCpy.imageOffset = { 0,0,0 };
	buffImgCpy.imageExtent = { _imageSize.x, _imageSize.y, 1 };

	vkCmdCopyBufferToImage(_cmdBuffer, _buffer->buffer, *_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffImgCpy);
}

VkCommandBuffer VK_Buffer::BeginSingleTimeCommand()
{
	VkCommandBuffer cmdBuffer = CreateCommandBuffer(*frameSync->GetCommandPool(), 1)[0];

	VkCommandBufferBeginInfo cmdBuffBeginInfo{};
	cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBuffBeginInfo.pNext = VK_NULL_HANDLE;
	cmdBuffBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	cmdBuffBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

	VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &cmdBuffBeginInfo), "Failed to begin command buffer !");

	return cmdBuffer;
}

void VK_Buffer::EndSingleTimeCommand(VkCommandBuffer _cmdBuffer)
{
	VK_CHECK(vkEndCommandBuffer(_cmdBuffer), "Failed to end command buffer !");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = VK_NULL_HANDLE;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
	submitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_cmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = VK_NULL_HANDLE;

	VK_CHECK(vkQueueSubmit(*device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE), "Failed to queue submit !");
	VK_CHECK(vkQueueWaitIdle(*device->GetGraphicsQueue()), "Failed queue wait idle !");

	vkFreeCommandBuffers(*device->GetDevice(), *frameSync->GetCommandPool(), 1, &_cmdBuffer);
}

void VK_Buffer::BeginCommandBuffer(VkCommandBuffer _cmdBuffer, VkCommandBufferUsageFlags _usage)
{
	VkCommandBufferBeginInfo cmdBuffBeginInfo{};
	cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBuffBeginInfo.pNext = VK_NULL_HANDLE;
	cmdBuffBeginInfo.flags = _usage;
	cmdBuffBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

	VK_CHECK(vkBeginCommandBuffer(_cmdBuffer, &cmdBuffBeginInfo), 
		"Failed to being command buffer !");
}
