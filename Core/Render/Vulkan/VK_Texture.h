#ifndef VK_TEXTURE__H
#define VK_TEXTURE__H
#include "VK_Image.h"

#include "../../Vma/VmaImpl.h"

#include "../../Vector/FwdVec2.h"
#include "../../LlynCore.h"

class VK_Texture : public VK_Image
{
private:
public:
	VkSampler m_sampler = VK_NULL_HANDLE;
public:
	void Init(VK_TextureInfo& _info);
	void Cleanup();
	static VkFormat GetFormat(const ImageFormat& _format);

	void BlitTo(VkCommandBuffer _cmd, VK_Image& _srcImg, VK_Image& _dstImage, const Vec2u& _srcTexSize, const Vec2u& _dstTexSize, uint32_t _srcMip, uint32_t
	            _dstMip, VkImageLayout _oldLayout);
	void BlitTo(VkCommandBuffer _cmd, VK_Image& _dstImg, const Vec2u& _dstTexSize, uint32_t _srcMip);
private:
	void CreateSampler(VkFilter _filter, VkSamplerMipmapMode _mipFilter, VkSamplerAddressMode _wrapMode);
	void GenerateMipMaps(VkCommandBuffer _cmdBuff, Vec2u& _texSize);
	static VkFilter GetFilter(const ImageFilter& _filter);

	VkSamplerMipmapMode GetMipFilter(const MipMapFilter& _filter);
	VkSamplerAddressMode GetWrapMode(const WrapMode& _wrapMode);
};

#endif