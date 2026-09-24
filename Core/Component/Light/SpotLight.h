#ifndef SPOT_LIGHT__H
#define SPOT_LIGHT__H

#include "../Component.h"
#include "../../LlynCore.h"

#include "../../Math/Math.h"
#include "../../Vector/Vec3.h"

struct SpotLight : public IComponent
{
	GPU_ALIGN Vec3f pos = { 0.0f, 0.0f, 0.0f };
	GPU_ALIGN Vec3f dir = { 0.0f , -1.0f, 0.0f };
	GPU_ALIGN Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	GPU_ALIGN Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f specular = { 1.0f, 1.0f, 1.0f };

	float cutOff = DegToRad(12.5f);
	float outerCutOff = DegToRad(17.5f);
};

#endif