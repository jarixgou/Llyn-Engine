#ifndef VK_TEXTURE__H
#define VK_TEXTURE__H
#include "VK_Image.h"

#include <VMA/vk_mem_alloc.h>

#include "../../LlynCore.h"

class VK_Texture : public VK_Image
{
private:
	VkSampler m_sampler = VK_NULL_HANDLE;
	VmaAllocation m_allocation = VK_NULL_HANDLE;
public:
	void Init(VK_TextureInfo& _info);

};

#endif