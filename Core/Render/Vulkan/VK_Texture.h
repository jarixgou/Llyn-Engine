#ifndef VK_TEXTURE__H
#define VK_TEXTURE__H
#include "VK_Image.h"

#include <VMA/vk_mem_alloc.h>

#include "../../Vector/FwdVec2.h"

class VK_Texture : public VK_Image
{
private:
	VkSampler m_sampler = VK_NULL_HANDLE;
	VmaAllocation m_allocation = VK_NULL_HANDLE;
public:
	void Init(VkImageUsageFlags _usage, VkFormat _format, VkImageLayout _layout, VkImageType _type, Vec2<uint32_t>& _texSize, void* data);

};

#endif