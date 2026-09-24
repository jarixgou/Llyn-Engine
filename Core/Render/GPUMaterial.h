#ifndef GPU_MATERIAL__H
#define GPU_MATERIAL__H

#include "../LlynCore.h"

#include "../Vector/Vec3.h"

struct GPUMaterial
{
	RessourceID baseID = TOMBSTONE_RESOURCE;
	RessourceID specularID = TOMBSTONE_RESOURCE;
	RessourceID normalID = TOMBSTONE_RESOURCE;

	GPU_ALIGN Vec3f baseColor = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f specularColor = { 1.0f, 1.0f, 1.0f };

	float smoothness = 0.5f;
};

#endif // !MATERIAL_GPU__H
