#ifndef POINT_LIGHT__H
#define POINT_LIGHT__H

#include "../Component.h"
#include "../../LlynCore.h"
#include "../../Vector/Vec3.h"

struct PointLight : public IComponent
{
	GPU_ALIGN Vec3f pos = { 0.0f, 0.0f, 0.0f };
	GPU_ALIGN Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	GPU_ALIGN Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	GPU_ALIGN Vec3f Specular = { 1.0f, 1.0f, 1.0f };

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.32f;

	float radius = 10;
};

#endif