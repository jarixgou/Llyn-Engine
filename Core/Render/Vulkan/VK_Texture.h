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

	static VkFormat GetFormat(ImageFormat _format, bool _sRGB);
	static VkFilter GetFilter(ImageFilter _filter);
	VkSamplerMipmapMode GetMipFilter(MipMapFilter _filter);
	VkSamplerAddressMode GetWrapMode(WrapMode _wrapMode);
};

#endif