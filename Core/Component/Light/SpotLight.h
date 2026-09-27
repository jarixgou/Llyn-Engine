#ifndef SPOT_LIGHT__H
#define SPOT_LIGHT__H

#include "../Component.h"
#include "../../LlynCore.h"

#include "../../Math/Math.h"
#include "../../Vector/Vec3.h"

struct SpotLight : public IComponent
{
	Vec3f pos = { 0.0f, 0.0f, 0.0f };
	Vec3f dir = { 0.0f , -1.0f, 0.0f };
	Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	Vec3f specular = { 1.0f, 1.0f, 1.0f };

	float cutOff = DegToRad(12.5f);
	float outerCutOff = DegToRad(17.5f);
};

#endif