#ifndef VK_DESCRIPTOR_MANAGER__H
#define VK_DESCRIPTOR_MANAGER__H
#include <deque>
#include <unordered_map>

#include "../../LlynCore.h"

#include "../UniformManager.h"

#include "VK_DescriptorLayoutHandle.h"
#include "VK_DescriptorHandle.h"
#include "VK_DescriptorLayoutHandleHash.h"

struct VK_PipelineHandle
{
	std::vector<VK_DescriptorHandle> m_descriptors;
	std::unordered_map<VK_DescriptorLayoutHandle, size_t, VK_DescriptorLayoutHandleHash> m_descriptorPools;
};

class VK_DescriptorManager : public IUniformManager
{
private:
	std::unordered_map<std::string, VK_PipelineHandle> m_pipelineHandle;

	std::deque<VkDescriptorImageInfo> m_imageInfos;
	std::deque<VkDescriptorBufferInfo> m_bufferInfos;
	std::vector<VkWriteDescriptorSet> m_writes;
public:
	void Init() override;
	void Cleanup();

	void Add(const std::string& _pipelineName, VK_DescriptorLayoutHandle _layoutHandle);
	void Adds(const std::string& _pipelineName, std::vector<VK_DescriptorLayoutHandle> _layoutHandles);

	std::vector<VkDescriptorSet> GetDescriptorSets(const std::string& _pipelineName, uint32_t _imageIndex);

	void StoreData(const std::string& _pipelineName, const std::string& _uniformName, void* _data, size_t _size, uint32_t _frameIndex) override;
	void StoreTexture(const std::string& _pipelineName, const std::string& _name, Texture* _texture, uint32_t _dstArray, uint32_t _frameIndex) override;
	void StoreTextures(const std::string& _pipelineName, const std::string& _uniformName, std::vector<Texture>& _textures, uint32_t _frameIndex) override;

	void StoreImage(const std::string& _pipelineName, const std::string& _uniformName, const uint32_t& _frameIndex, VK_Image& _image);

	void PushUniform() override;
private:
};

#endif