#ifndef POINT_LIGHT__H
#define POINT_LIGHT__H

#include "../Component.h"
#include "../../LlynCore.h"
#include "../../Vector/Vec3.h"

struct PointLight : public IComponent
{
	Vec3f pos = { 0.0f, 0.0f, 0.0f };
	Vec3f ambient = { 0.5f, 0.5f, 0.5f };
	Vec3f diffuse = { 1.0f, 1.0f, 1.0f };
	Vec3f specular = { 1.0f, 1.0f, 1.0f };

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.32f;

	float radius = 10;
};

#endif