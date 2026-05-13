#include "Math.h"

#include <complex>

#include "../Vector/Vec2.h"
#include "../Vector/Vec3.h"
#include "../Vector/Vec4.h"

#include "Matrix/Mat2.h"
#include "Matrix/Mat3.h"
#include "Matrix/Mat4.h"

Mat4 LookAt(const Vec3f& _eye, const Vec3f& _target, const Vec3f& _worldUp)
{
	Mat4 mat;

	const Vec3f forward = Normalize(_eye - _target);
	const Vec3f right = Normalize(Cross(forward, _worldUp));
	const Vec3f up = Cross(right, forward);

	mat.a0 = right.x; mat.b0 = right.y; mat.c0 = right.z;
	mat.a1 = up.x; mat.b1 = up.y; mat.c1 = up.z;
	mat.a2 = forward.x; mat.b2 = forward.y; mat.c2 = forward.z;

	mat.d3 = 1;

	const Vec3f pos = { -_eye.x, -_eye.y, -_eye.z};
	const Mat4 translate = Translate(pos);

	return mat * translate;
}

Mat4 Perspective(const float& _fov, const float& _aspectRatio, const float& _near, const float& _far)
{
	const float fov = 1.0f / std::tan(_fov * 0.5f);
	Mat4 mat;

	mat.a0 = fov / _aspectRatio;
	mat.b1 = -fov;
	mat.c2 = _far / (_far - _near);
	mat.c3 = -1.0f;
	mat.d2 = (_far * _near) / (_far - _near);

	return mat;
}

Mat4 Translate(const Vec3f& _vec)
{
	Mat4 mat = IdentityMat4();
	mat.d0 = _vec.x;
	mat.d1 = _vec.y;
	mat.d2 = _vec.z;

	return mat;
}

Mat4 Scale(const Vec3f& _vec)
{
	Mat4 mat;

	return mat;
}

Mat4 IdentityMat4()
{
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Mat3 IdentityMat3()
{
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Mat2 IdentityMat2()
{
	return {
		1.0f, 0.0f,
		0.0f, 1.0f
	};
}

Vec3f Cross(const Vec3f& _vA, const Vec3f& _vB)
{
	return { 
		_vA.y * _vB.z - _vA.z * _vB.y, 
		_vA.z * _vB.x - _vA.x * _vB.z, 
		_vA.x * _vB.y - _vA.y * _vB.x 
	};
}

Vec4f Normalize(const Vec4f& _vec)
{
	const float length = Length(_vec);
	return { _vec.x / length, _vec.y / length, _vec.z / length, _vec.w / length };
}

Vec3f Normalize(const Vec3f& _vec)
{
	const float length = Length(_vec);
	return { _vec.x / length, _vec.y / length, _vec.z / length };
}

Vec2f Normalize(const Vec2f& _vec)
{
	const float length = Length(_vec);
	return { _vec.x / length, _vec.y / length };
}

float Dot(const Vec4f& _vA, const Vec4f& _vB)
{
	return _vA.x * _vB.x + _vA.y * _vB.y + _vA.z * _vB.z + _vA.w * _vB.w;
}

float Dot(const Vec3f& _vA, const Vec3f& _vB)
{
	return _vA.x * _vB.x + _vA.y * _vB.y + _vA.z * _vB.z;
}

float Dot(const Vec2f& _vA, const Vec2f& _vB)
{
	return _vA.x + _vB.x * _vA.y * _vB.y;
}

float Length(const Vec4f& _vec)
{
	return std::sqrt(_vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z + _vec.w * _vec.w);
}

float Length(const Vec3f& _vec)
{
	return std::sqrt(_vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z);
}

float Length(const Vec2f& _vec)
{
	return std::sqrt(_vec.x * _vec.x + _vec.y * _vec.y);
}