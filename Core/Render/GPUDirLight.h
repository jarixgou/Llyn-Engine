#ifndef GPU_DIR_LIGHT__H
#define GPU_DIR_LIGHT__H

#include "../LlynCore.h"
#include "../Vector/Vec3.h"

struct GPUDirLight
{
	GPU_ALIGN Vec3f dir = { 0.0f, -1.0f, 0.0f };
	GPU_ALIGN Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	GPU_ALIGN Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f specular = { 1.0f, 1.0f, 1.0f };
};

#endif