#ifndef QUATERNIONS__H
#define QUATERNIONS__H

#include "../Vector/FwdVec3.h"

struct Mat4;

struct Quaternions
{
	float w = 1.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Mat4 GetMatrix() const;

	void SetEuler(Vec3f _vec);
	Vec3f GetEuler() const;
};

#endif