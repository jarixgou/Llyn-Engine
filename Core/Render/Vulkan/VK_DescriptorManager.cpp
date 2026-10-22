#include "VK_DescriptorManager.h"

#include "VK_Device.h"
#include "VK_LayoutInfo.h"
#include "VK_Texture.h"
#include "../../Logger.h"
#include "../../Asset/Texture.h"

void VK_DescriptorManager::Init()
{

}

void VK_DescriptorManager::Cleanup()
{
}

void VK_DescriptorManager::Add(const std::string& _pipelineName, VK_DescriptorLayoutHandle _layoutHandle)
{
	if (m_pipelineHandle[_pipelineName].m_descriptorPools.contains(_layoutHandle))
	{
		return;
	}

	VK_DescriptorHandle descHandle{};

	std::vector<VkDescriptorPoolSize> descPoolSizes(_layoutHandle.infos.size());
	bool bindless = false;
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

	VK_CHECK(vkCreateDescriptorPool(*VK_Device::Get().GetDevice(), &descPoolCreateInfo, VK_NULL_HANDLE, &descHandle.descriptorPools),
		"Failed to create descriptor pool !");

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _layoutHandle.layout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = VK_NULL_HANDLE;
	allocInfo.descriptorPool = descHandle.descriptorPools;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
	allocInfo.pSetLayouts = layouts.data();

	descHandle.descriptorSet.resize(MAX_FRAMES_IN_FLIGHT);
	VK_CHECK(vkAllocateDescriptorSets(*VK_Device::Get().GetDevice(), &allocInfo, descHandle.descriptorSet.data()),
		"Failed to allocate descriptor sets !");

	descHandle.shaderBuffer.resize(_layoutHandle.infos.size());
	m_pipelineHandle[_pipelineName].m_descriptorPools[_layoutHandle] = m_pipelineHandle[_pipelineName].m_descriptors.size();
	m_pipelineHandle[_pipelineName].m_descriptors.emplace_back(descHandle);
}

void VK_DescriptorManager::Adds(const std::string& _pipelineName, std::vector<VK_DescriptorLayoutHandle> _layoutHandles)
{
	for (int i = 0; i < _layoutHandles.size(); ++i)
	{
		Add(_pipelineName, _layoutHandles[i]);
	}
}

std::vector<VkDescriptorSet> VK_DescriptorManager::GetDescriptorSets(const std::string& _pipelineName, uint32_t _imageIndex)
{
	if (!m_pipelineHandle.contains(_pipelineName))
	{
		LOGGER_ERROR("Wrong pipeline name : " + _pipelineName);
		return std::vector<VkDescriptorSet>();
	}

	std::vector<VkDescriptorSet> descSets(m_pipelineHandle[_pipelineName].m_descriptors.size());
	for (size_t i = 0; i < m_pipelineHandle[_pipelineName].m_descriptors.size(); ++i)
	{
		descSets[i] = m_pipelineHandle[_pipelineName].m_descriptors[i].descriptorSet[_imageIndex];
	}

	return descSets;
}

void VK_DescriptorManager::StoreData(const std::string& _pipelineName, const std::string& _uniformName, void* _data, size_t _size, uint32_t _frameIndex)
{
	if (!m_pipelineHandle.contains(_pipelineName))
	{
		LOGGER_ERROR("Wrong pipeline name : " + _pipelineName + " for " + _uniformName);
		return;
	}

	for (auto& descriptorPool : m_pipelineHandle[_pipelineName].m_descriptorPools)
	{
		for (size_t i = 0; i < descriptorPool.first.infos.size(); ++i)
		{
			const VK_LayoutInfo layoutInfo = descriptorPool.first.infos[i];
			VK_DescriptorHandle* descHandle = &m_pipelineHandle[_pipelineName].m_descriptors[descriptorPool.second];

			if (layoutInfo.name == _uniformName)
			{
				const bool isCreated = descHandle->shaderBuffer[i].IsCreated(_frameIndex);

				descHandle->shaderBuffer[i].StoreData(_data, _size, layoutInfo.type, _frameIndex);

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

void VK_DescriptorManager::StoreTexture(const std::string& _pipelineName, const std::string& _name, Texture* _texture, uint32_t _dstArray, uint32_t _frameIndex)
{
	LLYN_ASSERT(_texture != nullptr);
}

void VK_DescriptorManager::StoreTextures(const std::string& _pipelineName, const std::string& _uniformName, std::vector<Texture>& _textures, uint32_t _frameIndex)
{
	if (!m_pipelineHandle.contains(_pipelineName))
	{
		LOGGER_ERROR("Wrong pipeline name : " + _pipelineName + " for " + _uniformName);
		return;
	}

	const VK_LayoutInfo* layoutInfo = nullptr;
	VK_DescriptorHandle* descriptorHandle = nullptr;
	for (auto& descriptorPool : m_pipelineHandle[_pipelineName].m_descriptorPools)
	{
		for (int i = 0; i < descriptorPool.first.infos.size(); ++i)
		{
			if (descriptorPool.first.infos[i].name == _uniformName)
			{
				descriptorHandle = &m_pipelineHandle[_pipelineName].m_descriptors[descriptorPool.second];
				layoutInfo = &descriptorPool.first.infos[i];
			}
		}
	}

	m_writes.reserve(m_writes.size() + _textures.size());
	for (size_t i = 0; i < _textures.size(); ++i)
	{
		VK_Texture* texture = _textures[i].GetVKTexture();
		VkDescriptorImageInfo& imageInfo = m_imageInfos.emplace_back();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->m_imageView;
		imageInfo.sampler = texture->m_sampler;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = VK_NULL_HANDLE;
		write.dstSet = descriptorHandle->descriptorSet[_frameIndex];
		write.dstBinding = layoutInfo->binding;
		write.dstArrayElement = static_cast<uint32_t>(i);
		write.descriptorCount = 1;
		write.descriptorType = layoutInfo->type;
		write.pImageInfo = &imageInfo;
		write.pBufferInfo = VK_NULL_HANDLE;
		write.pTexelBufferView = VK_NULL_HANDLE;

		m_writes.emplace_back(write);
	}
}

void VK_DescriptorManager::StoreImage(const std::string& _pipelineName, const std::string& _uniformName,
                                      const uint32_t& _frameIndex, VK_Image& _image)
{
	if (!m_pipelineHandle.contains(_pipelineName))
	{
		LOGGER_ERROR("Wrong pipeline name : " + _pipelineName + " for " + _uniformName);
		return;
	}

	const VK_LayoutInfo* layoutInfo = nullptr;
	VK_DescriptorHandle* descriptorHandle = nullptr;
	for (auto& descriptorPool : m_pipelineHandle[_pipelineName].m_descriptorPools)
	{
		for (int i = 0; i < descriptorPool.first.infos.size(); ++i)
		{
			if (descriptorPool.first.infos[i].name == _uniformName)
			{
				descriptorHandle = &m_pipelineHandle[_pipelineName].m_descriptors[descriptorPool.second];
				layoutInfo = &descriptorPool.first.infos[i];
			}
		}
	}

	VkDescriptorImageInfo& imageInfo = m_imageInfos.emplace_back();
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = _image.m_imageView;
	imageInfo.sampler = VK_NULL_HANDLE;

	VkWriteDescriptorSet write;
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = VK_NULL_HANDLE;
	write.dstSet = descriptorHandle->descriptorSet[_frameIndex];
	write.dstBinding = layoutInfo->binding;
	write.dstArrayElement = 0;
	write.descriptorCount = 1;
	write.descriptorType = layoutInfo->type;
	write.pImageInfo = &imageInfo;
	write.pBufferInfo = VK_NULL_HANDLE;
	write.pTexelBufferView = VK_NULL_HANDLE;

	m_writes.emplace_back();
}

void VK_DescriptorManager::PushUniform()
{
	if (!m_writes.empty())
	{
		vkUpdateDescriptorSets(*VK_Device::Get().GetDevice(), static_cast<uint32_t>(m_writes.size()),
		                       m_writes.data(), 0, VK_NULL_HANDLE);
	}

	m_imageInfos.clear();
	m_bufferInfos.clear();
	m_writes.clear();
}
