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

	void Normalize();

	Mat4 GetMatrix() const;

	void SetEuler(Vec3f _vec);
	Vec3f GetEuler() const;

	void SetAxisAngle(const Vec3f& _axis, float _angle);

	Vec3f Rotate(const Vec3f& _v);

	Vec3f GetForward() const;
	Vec3f GetRight();
	Vec3f GetUp();

	Quaternions operator*(const Quaternions& _other);
	Quaternions& operator*=(const Quaternions& _other);
};

#endif