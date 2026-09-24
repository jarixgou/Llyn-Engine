#ifndef TRANSFORM__H
#define TRANSFORM__H

#include "Component.h"
#include "../Math/Quaternions.h"
#include "../Vector/Vec3.h"

struct Mat4;

struct Transform : public IComponent
{
	Quaternions rot = { 1.0f, 0.0f, 0.0f, 0.0f };
	Vec3f pos = { 0.0f, 0.0f, 0.0f };
	Vec3f scale = { 1.0f, 1.0f, 1.0f };

	Mat4 GetMatrix() const;

	Transform operator+(const Transform& _other);
	Transform& operator+=(const Transform& _other);

	Transform operator*(const Transform& _other);
	Transform operator*=(const Transform& _other);
};

#endif