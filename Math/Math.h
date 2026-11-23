#ifndef MATH__H
#define MATH__H
#include "Vector/FwdVec4.h"
#include "Vector/FwdVec3.h"

template<typename T>
T Clamp(const T& _v, const T& _a, const T& _b)
{
	if (_v < _a)
	{
		return _a;
	}
	else if (_v > _b)
	{
		return _b;
	}

	return _v;
}

Vec3f DegToRad(const Vec3f& _v);
Vec3f RadToDeg(const Vec3f& _v);

Vec4f DegToRad(const Vec4f& _v);
Vec4f RadToDeg(const Vec4f& _v);

#endif