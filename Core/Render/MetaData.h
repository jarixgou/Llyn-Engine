#ifndef META_DATA__H
#define META_DATA__H

#include "../LlynCore.h"
#include "../Vector/Vec3.h"

struct MetaData
{
	uint32_t indexOffset = 0;
	uint32_t indexCount = 0;
	uint32_t vertexOffset = 0;

	RessourceID materialID = TOMBSTONE_RESOURCE;
};

#endif // !META_DATA__H
