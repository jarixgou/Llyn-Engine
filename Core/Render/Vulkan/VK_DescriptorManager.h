#ifndef VK_DESCRIPTOR_MANAGER__H
#define VK_DESCRIPTOR_MANAGER__H
#include <deque>
#include <unordered_map>

#include "../../LlynCore.h"

#include "../UniformManager.h"

#include "VK_DescriptorLayoutHandle.h"
#include "VK_DescriptorHandle.h"
#include "VK_DescriptorLayoutHandleHash.h"


class VK_DescriptorManager : public IUniformManager
{
private:
	VK_Device* m_device = nullptr;

	std::vector<VK_DescriptorHandle> m_descriptors;
	std::unordered_map<VK_DescriptorLayoutHandle, size_t, VK_DescriptorLayoutHandleHash> m_descriptorPools;
	VK_DescriptorLayoutHandle bindlessLayout;

	std::deque<VkDescriptorImageInfo> m_imageInfos;
	std::deque<VkDescriptorBufferInfo> m_bufferInfos;
	std::vector<VkWriteDescriptorSet> m_writes;
public:
	void Init() override;
	void Cleanup();

	void SetDevice(VK_Device* _device);

	void Add(VK_DescriptorLayoutHandle _layoutHandle);
	void Adds(std::vector<VK_DescriptorLayoutHandle> _layoutHandles);

	std::vector<VkDescriptorSet> GetDescriptorSets(uint32_t _imageIndex);

	void StoreData(std::string _name, void* _data, size_t _size, uint32_t _frameIndex) override;
	void StoreTexture(Texture* _texture, uint32_t _dstArray, uint32_t _frameIndex) override;
	void StoreTextures(std::vector<Texture>& _textures, uint32_t _frameIndex) override;
	void PushUniform() override;
private:
};

#endif