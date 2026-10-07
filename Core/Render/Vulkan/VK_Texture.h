#ifndef VK_TEXTURE__H
#define VK_TEXTURE__H
#include "VK_Image.h"

#include <VMA/vk_mem_alloc.h>

#include "../../Vector/FwdVec2.h"
#include "../../LlynCore.h"

class VK_Texture : public VK_Image
{
private:
	VkSampler m_sampler = VK_NULL_HANDLE;
	VmaAllocation m_allocation = VK_NULL_HANDLE;
public:
	void Init(VK_TextureInfo& _info);
private:
	void CreateSampler(VkFilter _filter, VkSamplerMipmapMode _mipFilter, VkSamplerAddressMode _wrapMode);
	void GenarteMipMaps(VkCommandBuffer _cmdBuff, Vec2u& _texSize);

	static VkFormat GetFormat(ImageFormat _format, bool _sRGB);
	static VkFilter GetFilter(ImageFilter _filter);
	VkSamplerMipmapMode GetMipFilter(MipMapFilter _filter);
	VkSamplerAddressMode GetWrapMode(WrapMode _wrapMode);
};

#endif