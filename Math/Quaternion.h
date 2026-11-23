#ifndef QUATERNION__H
#define QUATERNION__H

#include "Vector/FwdVec3.h"
struct Mat4;

struct Quaternion
{
	float w = 1.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Quaternion();
	Quaternion(const float& _x, const float& _y, const float& _z);
	Quaternion(const float& _w, const float& _x, const float& _y, const float& _z);

	Quaternion& operator*=(const Quaternion& _a);
	Quaternion operator*(const Quaternion& _a);

	bool operator==(const Quaternion& _a) const;
	bool operator!=(const Quaternion& _a) const;

	Vec3f GetEuler();
	Vec3f SetEuler(const Vec3f& _angle);

	Quaternion Normalize();

	static Quaternion Identity();

	void SetFromToRotation(const Vec3f& _fromDirection, const Vec3f& _toDirection);
	void SetLookRotation(const Vec3f& _view, const Vec3f& _up);
	void ToAngleAxis(const float& _angle, const Vec3f& _axis);

	Mat4 GetMatrix();

	static float Angle(const Quaternion& _a, const Quaternion& _b);
	static Quaternion AngleAxis(const float& _angle, const Vec3f& _axis);
	static float Dot(const Quaternion& _a, const Quaternion& _b);
	static Quaternion Euler(const Vec3f& _angle);
	static Quaternion FromToRotation(const Vec3f& _fromDirection, const Vec3f& _toDirection);
	static Quaternion Inverse(const Quaternion& _a);
	static Quaternion Lerp(const Quaternion& _a, const Quaternion& _b, const float& _t);
	static Quaternion LookRotation(const Vec3f& _view, const Vec3f& _up);
	static Quaternion RotateTowards(const Quaternion& _from, const Quaternion& _to, const float& _maxDegreesDelta);
	static Quaternion Slerp(const Quaternion& _a, const Quaternion& _b, const float& _t);
};

#endif