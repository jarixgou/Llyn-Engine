#ifndef VERTEX__H
#define VERTEX__H

#include "LlynCore.h"

#include "Vector/Vec2.h"
#include "Vector/Vec3.h"

struct Vertex
{
	GPU_ALIGN Vec3f pos = { 0.0f, 0.0f, 0.0f };

	GPU_ALIGN Vec3f normal = { 0.0f, 0.0f, 0.0f };
	GPU_ALIGN Vec3f tangent = { 0.0f, 0.0f, 0.0f };
	GPU_ALIGN Vec3f bitangent = { 0.0f, 0.0f, 0.0f };

	GPU_ALIGN Vec3f color = { 1.0f, 1.0f, 1.0f };

	GPU_ALIGN Vec2f uv0 = { 0.0f, 0.0f };
	Vec2f uv1 = { 0.f, 0.0f };
};

#endif