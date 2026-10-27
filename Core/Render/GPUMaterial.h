#ifndef GPU_MATERIAL__H
#define GPU_MATERIAL__H

#include "../LlynCore.h"

#include "../Vector/Vec3.h"

#include "../Asset/MaterialMode.h"

struct GPUMaterial
{
	MaterialMode mode = MaterialMode::SPECULAR;
	RessourceID baseID = TOMBSTONE_RESOURCE;
	RessourceID specularID = TOMBSTONE_RESOURCE;
	RessourceID metallicID = TOMBSTONE_RESOURCE;
	RessourceID normalID = TOMBSTONE_RESOURCE;
	RessourceID emissiveID = TOMBSTONE_RESOURCE;
	RessourceID aoID = TOMBSTONE_RESOURCE;

	GPU_ALIGN Vec3f baseColor = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f specularColor = { 1.0f, 1.0f, 1.0f };

	float smoothness = 0.5f;

	GPU_ALIGN Vec3f emissiveColor = { 1.0f, 1.0f, 1.0f };
};

#endif // !MATERIAL_GPU__H
