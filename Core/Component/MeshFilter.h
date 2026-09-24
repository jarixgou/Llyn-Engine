#ifndef MESH_FILTER__H
#define MESH_FILTER__H

#include "Component.h"

class MeshFilter : public IComponent
{
public:
	uint32_t vertexOffset = 0;
	uint32_t vertexCount = 0;

	uint32_t indexOffset = 0;
	uint32_t indexCount = 0;
};

#endif