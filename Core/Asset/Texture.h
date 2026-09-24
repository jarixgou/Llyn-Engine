#ifndef TEXTURE__H
#define TEXTURE__H

#include <vulkan/vulkan_raii.hpp>

#include "../LlynCore.h"

#include "Ressource.h"
#include "../Vector/FwdVec2.h"

struct TextureConfig
{
	VkImageType imageType = VK_IMAGE_TYPE_2D;
	VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;
	VkSamplerAddressMode wrapMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkFilter samplerFilter = VK_FILTER_NEAREST;
	VkSamplerMipmapMode samplerMimapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	bool sRGB = true;
	bool mipmap = true;
};

class Texture : public IRessource
{
private:
	static VK_Device* m_device;

	TextureConfig m_config;

	uint32_t m_mipLevels = 0;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkImage m_image = VK_NULL_HANDLE;
	VkImageView m_imageView = VK_NULL_HANDLE;
	VkSampler m_sampler = VK_NULL_HANDLE;
public:
	Texture() = default;
	~Texture() override;

	bool Load(const std::string& _filePath) override;
	void Unload() override;

	bool Add(const std::string& _filePath, IRessource* _ressource) override;

	VkImageView* GetImageView();
	VkSampler* GetSampler();

	static void SetDevice(VK_Device* _device);

	static std::pair<VkImage, VkDeviceMemory> CreateImage(Vec2<uint32_t> _imageSize, uint32_t _mipLevels,
	                                                      VkImageType _imageType, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
	static VkImageView CreateImageView(uint32_t _mipLevels, VkImageViewType _imageType, VkImage* _image, VkFormat _format, VkImageAspectFlags _aspect);
	static void GenerateMipMaps(const VK_Device* _device, VkCommandBuffer _cmdBuffer, VkImage* _image, VkFormat _format, Vec2<uint32_t> _imageSize, uint32_t _mipLevels);
	static VkSampler CreateSampler(VkFilter _filter, VkSamplerMipmapMode _samplerMipmap, VkSamplerAddressMode _samplerAddressMode);
	static void TransitionImageLayout(VkCommandBuffer _cmdBuffer, const VkImage* _image, uint32_t _mipLevels,
	                                  VkImageLayout _oldLayout, VkImageLayout _newLayout,
	                                  VkAccessFlags2 _srcAccessMask, VkAccessFlags2 _dstAccessMask,
	                                  VkPipelineStageFlags2 _srcStageMask, VkPipelineStageFlags2 _dstStageMask, VkImageAspectFlags _imageAspectFlags);
private:
	void LoadConfig(const std::string& _filePath);
};

#endif
