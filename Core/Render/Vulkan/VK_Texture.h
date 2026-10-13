#ifndef VK_TEXTURE__H
#define VK_TEXTURE__H
#include "VK_Image.h"

#include "../../Vma/VmaImpl.h"

#include "../../Vector/FwdVec2.h"
#include "../../LlynCore.h"

class VK_Texture : public VK_Image
{
public:
	VkSampler m_sampler = VK_NULL_HANDLE;
public:
	void Init(VK_TextureInfo& _info);
	void Cleanup();
private:
	void CreateSampler(VkFilter _filter, VkSamplerMipmapMode _mipFilter, VkSamplerAddressMode _wrapMode);
	void GenerateMipMaps(VkCommandBuffer _cmdBuff, Vec2u& _texSize);

	static VkFormat GetFormat(const ImageFormat& _format, const bool& _sRGB);
	static VkFilter GetFilter(const ImageFilter& _filter);
	VkSamplerMipmapMode GetMipFilter(const MipMapFilter& _filter);
	VkSamplerAddressMode GetWrapMode(const WrapMode& _wrapMode);
};

#endif