#ifndef TRANSFORM__H
#define TRANSFORM__H

#include "Quaternions.h"
#include "Vector/Vec3.h"

struct Mat4;

struct Transform
{
	Quaternions rot = { 1.0f, 0.0f, 0.0f, 0.0f };
	Vec3f pos = { 0.0f, 0.0f, 0.0f };
	Vec3f scale = { 0.0f, 0.0f, 0.0f };

	Mat4 GetMatrix();
};

#endif