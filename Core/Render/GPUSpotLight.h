#ifndef GPU_SPOT_LIGHT__H
#define GPU_SPOT_LIGHT__H

#include "../LlynCore.h"
#include "../Vector/Vec3.h"
#include "../Math/Math.h"

struct GPUSpotLight
{
	GPU_ALIGN Vec3f pos = { 0.0f, 0.0f, 0.0f };
	GPU_ALIGN Vec3f dir = { 0.0f, -1.0f, 0.0f };
	GPU_ALIGN Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	GPU_ALIGN Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f specular = { 1.0f, 1.0f, 1.0f };

	float cutOff = DegToRad(12.5f);
	float outerCutfOff = DegToRad(17.5f);
};

#endif