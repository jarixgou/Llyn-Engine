#include "VK_ShaderBuffer.h"

#include "VK_Device.h"
#include "VK_LayoutInfo.h"

void VK_ShaderBuffer::CreateUniform(VK_Device* _device, const VK_LayoutInfo& _layoutInfo, void* _data)
{
	LLYN_ASSERT(_device != nullptr);

	buffers.resize(MAX_FRAMES_IN_FLIGHT);

	bufferUsage = _layoutInfo.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ? 
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (bufferUsage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		{
			buffers[i].CreateUBO(_layoutInfo.count);

			VK_CHECK(vkMapMemory(*_device->GetDevice(), buffers[i].memory, 0, _layoutInfo.count, 0, &_data),
				"Failed to map memory");
			buffersMapped.emplace_back(_data);
		}
		else
		{
			buffers[i].CreateSSBO(nullptr, _layoutInfo.count);
		}
	}
}

void VK_ShaderBuffer::StoreData(const VK_Device* _device, void* _data, size_t _size, VkDescriptorType _type, uint32_t _frameIndex)
{
	LLYN_ASSERT(_device != nullptr);

	if (!createdFrame[_frameIndex])
	{
		bufferUsage = _type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ?
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		buffers.resize(MAX_FRAMES_IN_FLIGHT);
		if (bufferUsage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		{
			buffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
		}

		if (bufferUsage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		{
			buffers[_frameIndex] = VK_Buffer::CreateUBO(_size);

			void* mem;
			VK_CHECK(vkMapMemory(*_device->GetDevice(), buffers[_frameIndex].memory, 0, _size, 0, &mem),
				"Failed to map memory !");
			buffersMapped[_frameIndex] = mem;

			memcpy(buffersMapped[_frameIndex], _data, _size);
		}
		else if (bufferUsage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		{
			buffers[_frameIndex] = VK_Buffer::CreateSSBO(_data, _size);
		}

		createdFrame[_frameIndex] = true;
	}
	else
	{
		if (bufferUsage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		{
			LLYN_ASSERT(bufferSize == _size);

			memcpy(buffersMapped[_frameIndex], _data, _size);
		}
		else if (bufferUsage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		{
			buffers[_frameIndex].Cleanup();
			buffers[_frameIndex] = VK_Buffer::CreateSSBO(_data, _size);
		}
	}
}

bool VK_ShaderBuffer::IsCreated(uint32_t _frameIndex) const
{
	return createdFrame[_frameIndex];
}
