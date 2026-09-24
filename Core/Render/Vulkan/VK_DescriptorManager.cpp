#include "VK_DescriptorManager.h"

#include "VK_Device.h"
#include "VK_LayoutInfo.h"
#include "../../Asset/Texture.h"

void VK_DescriptorManager::Init()
{
	m_descriptorPools.clear();
	m_descriptors.clear();
}

void VK_DescriptorManager::Cleanup()
{
}

void VK_DescriptorManager::SetDevice(VK_Device* _device)
{
	m_device = _device;
}

void VK_DescriptorManager::Add(VK_DescriptorLayoutHandle _layoutHandle)
{
	if (m_descriptorPools.contains(_layoutHandle))
	{
		return;
	}

	VK_DescriptorHandle descHandle{};

	std::vector<VkDescriptorPoolSize> descPoolSizes(_layoutHandle.infos.size());
	bool bindless = false;
	uint32_t bindlessCount = 1;
	for (size_t i = 0; i < _layoutHandle.infos.size(); ++i)
	{
		VkDescriptorPoolSize descPoolSize;
		descPoolSize.type = _layoutHandle.infos[i].type;
		descPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT * _layoutHandle.infos[i].count;

		descPoolSizes[i] = descPoolSize;

		if (_layoutHandle.infos[i].bindless)
		{
			bindless = true;
		}
	}

	const VkDescriptorPoolCreateFlags descPoolFlags = bindless ?
		VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	VkDescriptorPoolCreateInfo descPoolCreateInfo{};
	descPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descPoolCreateInfo.pNext = VK_NULL_HANDLE;
	descPoolCreateInfo.flags = descPoolFlags;
	descPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
	descPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descPoolSizes.size());
	descPoolCreateInfo.pPoolSizes = descPoolSizes.data();

	VK_CHECK(vkCreateDescriptorPool(*m_device->GetDevice(), &descPoolCreateInfo, VK_NULL_HANDLE, &descHandle.descriptorPools),
		"Failed to create descriptor pool !");

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _layoutHandle.layout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = VK_NULL_HANDLE;
	allocInfo.descriptorPool = descHandle.descriptorPools;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	descHandle.descriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
	VK_CHECK(vkAllocateDescriptorSets(*m_device->GetDevice(), &allocInfo, descHandle.descriptorSet.data()),
		"Failed to allocate descriptor sets !");

	descHandle.shaderBuffer.resize(_layoutHandle.infos.size());
	m_descriptorPools[_layoutHandle] = m_descriptors.size();
	m_descriptors.emplace_back(descHandle);

	if (bindless)
	{
		bindlessLayout = _layoutHandle;
	}
}

void VK_DescriptorManager::Adds(std::vector<VK_DescriptorLayoutHandle> _layoutHandles)
{
	for (int i = 0; i < _layoutHandles.size(); ++i)
	{
		Add(_layoutHandles[i]);
	}
}

std::vector<VkDescriptorSet> VK_DescriptorManager::GetDescriptorSets(uint32_t _imageIndex)
{
	std::vector<VkDescriptorSet> descSets(m_descriptors.size());
	for (size_t i = 0; i < m_descriptors.size(); ++i)
	{
		descSets[i] = m_descriptors[i].descriptorSet[_imageIndex];
	}

	return descSets;
}

void VK_DescriptorManager::StoreData(std::string _name, void* _data, size_t _size, uint32_t _frameIndex)
{
	for (auto& descriptorPool : m_descriptorPools)
	{
		for (size_t i = 0; i < descriptorPool.first.infos.size(); ++i)
		{
			const VK_LayoutInfo layoutInfo = descriptorPool.first.infos[i];
			VK_DescriptorHandle* descHandle = &m_descriptors[descriptorPool.second];

			if (layoutInfo.name == _name)
			{
				const bool isCreated = descHandle->shaderBuffer[i].IsCreated(_frameIndex);

				descHandle->shaderBuffer[i].StoreData(m_device, _data, _size, layoutInfo.type, _frameIndex);

				if (!isCreated || layoutInfo.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
				{
					VkDescriptorBufferInfo& bufferInfo = m_bufferInfos.emplace_back();
					bufferInfo.buffer = descHandle->shaderBuffer[i].buffers[_frameIndex].buffer;
					bufferInfo.offset = 0;
					bufferInfo.range = _size;

					VkWriteDescriptorSet write{};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.dstSet = descHandle->descriptorSet[_frameIndex];
					write.dstBinding = layoutInfo.binding;
					write.dstArrayElement = 0;
					write.descriptorCount = 1;
					write.descriptorType = layoutInfo.type;
					write.pBufferInfo = &bufferInfo;

					m_writes.emplace_back(write);
				}
			}
		}
	}
}

void VK_DescriptorManager::StoreTexture(Texture* _texture, uint32_t _dstArray, uint32_t _frameIndex)
{
	LLYN_ASSERT(_texture != nullptr);

	/*VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = *_texture->GetImageView();
	imageInfo.sampler = *_texture->GetSampler();

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		std::vector<VkWriteDescriptorSet> writes(MAX_FRAMES_IN_FLIGHT);

		writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[0].pNext = VK_NULL_HANDLE;
		writes[0].dstSet = m_descriptors[m_descriptorPools[bindlessLayout]].descriptorSet[i];
		writes[0].dstBinding = 0;
		writes[0].dstArrayElement = _dstArray;
		writes[0].descriptorCount = 1;
		writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[0].pImageInfo = &imageInfo;
		writes[0].pBufferInfo = VK_NULL_HANDLE;
		writes[0].pTexelBufferView = VK_NULL_HANDLE;

		writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[1].pNext = VK_NULL_HANDLE;
		writes[1].dstSet = m_descriptors[m_descriptorPools[bindlessLayout]].descriptorSet[i];
		writes[1].dstBinding = 0;
		writes[1].dstArrayElement = _dstArray;
		writes[1].descriptorCount = 1;
		writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[1].pImageInfo = &imageInfo;
		writes[1].pBufferInfo = VK_NULL_HANDLE;
		writes[1].pTexelBufferView = VK_NULL_HANDLE;

		vkUpdateDescriptorSets(*m_device->GetDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, VK_NULL_HANDLE);
	}*/
}

void VK_DescriptorManager::StoreTextures(std::vector<Texture>& _textures, uint32_t _frameIndex)
{
	if (!m_descriptorPools.contains(bindlessLayout))
	{
		return;
	}

	m_writes.reserve(m_writes.size() + _textures.size());
	for (size_t i = 0; i < _textures.size(); ++i)
	{
		VkDescriptorImageInfo& imageInfo = m_imageInfos.emplace_back();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *_textures[i].GetImageView();
		imageInfo.sampler = *_textures[i].GetSampler();

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = VK_NULL_HANDLE;
		write.dstSet = m_descriptors[m_descriptorPools[bindlessLayout]].descriptorSet[_frameIndex];
		write.dstBinding = 0;
		write.dstArrayElement = static_cast<uint32_t>(i);
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &imageInfo;
		write.pBufferInfo = VK_NULL_HANDLE;
		write.pTexelBufferView = VK_NULL_HANDLE;

		m_writes.emplace_back(write);
	}
}

void VK_DescriptorManager::PushUniform()
{
	if (!m_writes.empty())
	{
		vkUpdateDescriptorSets(*m_device->GetDevice(), static_cast<uint32_t>(m_writes.size()),
		                       m_writes.data(), 0, VK_NULL_HANDLE);
	}

	m_imageInfos.clear();
	m_bufferInfos.clear();
	m_writes.clear();
}