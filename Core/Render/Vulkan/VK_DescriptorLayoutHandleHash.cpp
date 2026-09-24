#include "VK_DescriptorLayoutHandleHash.h"

#include "VK_DescriptorLayoutHandle.h"
#include "VK_LayoutInfo.h"

size_t VK_DescriptorLayoutHandleHash::operator()(const VK_DescriptorLayoutHandle& _layout) const
{
	size_t hSeed = _layout.infos.size();
	for (size_t i = 0; i < _layout.infos.size(); ++i)
	{
		const size_t hName = std::hash<std::string>()(_layout.infos[i].name);
		const size_t hBinding = std::hash<uint32_t>()(_layout.infos[i].binding);
		const size_t hOffset = std::hash<uint32_t>()(_layout.infos[i].offset);
		const size_t hCount = std::hash<uint32_t>()(_layout.infos[i].count);
		const size_t hType = std::hash<uint32_t>()(_layout.infos[i].type);
		const size_t hStageFlags = std::hash<uint32_t>()(_layout.infos[i].stageFlags);
		const size_t hBindless = std::hash<uint32_t>()(_layout.infos[i].bindless);

		hSeed ^= hName ^ (hBinding << 1) ^ (hOffset << 2) ^ (hCount << 3) ^ (hType << 4)
		^ (hStageFlags << 5) ^ (hBindless << 6);
	}
	return hSeed;
}
