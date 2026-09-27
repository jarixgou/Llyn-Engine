#ifndef GPU_POINT_LIGHT__H
#define GPU_POINT_LIGHT__H

#include "../LlynCore.h"
#include "../Vector/Vec3.h"

struct GPUPointLight
{
	GPU_ALIGN Vec3f pos = { 0.0f, 0.0f, 0.0f };
	GPU_ALIGN Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	GPU_ALIGN Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f specular = { 1.0f, 1.0f, 1.0f };

	GPU_ALIGN float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.32f;

	float radius = 10.0f;
};

#endif