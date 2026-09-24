#include "Math.h"

#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../Vector/Vec2.h"
#include "../Vector/Vec3.h"
#include "../Vector/Vec4.h"

#include "Matrix/Mat2.h"
#include "Matrix/Mat3.h"
#include "Matrix/Mat4.h"

#include "Quaternions.h"

Quaternions LookAtQuat(const Vec3f& _forward, Vec3f& _up)
{
	Quaternions quat;

	Mat4 mat = LookAtMat(_forward, _up);

	const float trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
	if (trace > 0.0f)
	{
		const float s = sqrtf(trace + 1.0f) * 2;

		quat.w = 0.25f * s;
		quat.x = (mat.m[1][2] - mat.m[2][1]) / s;
		quat.y = (mat.m[2][0] - mat.m[0][2]) / s;
		quat.z = (mat.m[0][1] - mat.m[1][0]) / s;
	}
	else if (mat.m[0][0] > mat.m[1][1] && mat.m[0][0] > mat.m[2][2])
	{
		const float s = sqrtf(1.0f + mat.m[0][0] - mat.m[1][1] - mat.m[2][2]) * 2;

		quat.w = (mat.m[1][2] - mat.m[2][1]) / s;
		quat.x = 0.25f * s;
		quat.y = (mat.m[0][1] + mat.m[1][0]) / s;
		quat.z = (mat.m[0][2] + mat.m[2][0]) / s;
	}
	else if (mat.m[1][1] > mat.m[2][2])
	{
		const float s = sqrtf(1.0f + mat.m[1][1] - mat.m[0][0] - mat.m[2][2]) * 2;

		quat.w = (mat.m[2][0] - mat.m[0][2]) / s;
		quat.x = (mat.m[0][1] + mat.m[1][0]) / s;
		quat.y = 0.25f * s;
		quat.z = (mat.m[1][2] + mat.m[2][1]) / s;
	}
	else
	{
		const float s = sqrt(1.0f + mat.m[2][2] - mat.m[0][0] - mat.m[1][1]) * 2;

		quat.w = (mat.m[0][1] - mat.m[1][0]) / s;
		quat.x = (mat.m[0][2] + mat.m[2][0]) / s;
		quat.y = (mat.m[1][2] + mat.m[2][1]) / s;
		quat.z = 0.25f * s;
	}

	return quat;
}

Mat4 LookAtMat(const Vec3f& _forward, const Vec3f& _up)
{
	const Vec3f forward = Normalize(_forward);
	const Vec3f right = Normalize(Cross(_up, forward));
	const Vec3f up = Cross(forward, right);

	Mat4 mat;
	mat.m[0][0] = right.x;   mat.m[0][1] = up.x;   mat.m[0][2] = forward.x;   mat.m[0][3] = 0.0f;
	mat.m[1][0] = right.y;   mat.m[1][1] = up.y;   mat.m[1][2] = forward.y;   mat.m[1][3] = 0.0f;
	mat.m[2][0] = right.z;   mat.m[2][1] = up.z;   mat.m[2][2] = forward.z;   mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f;      mat.m[3][1] = 0.0f;   mat.m[3][2] = 0.0f;        mat.m[3][3] = 1.0f;

	return mat;
}

Mat4 View(
	const Vec3f& _eye,
	const Vec3f& _target,
	const Vec3f& _worldUp)
{
	Mat4 mat = IdentityMat4();

	const Vec3f forward = Normalize(_target - _eye);
	const Vec3f right = Normalize(Cross(_worldUp, forward));
	const Vec3f up = Cross(forward, right);

	mat.m[0][0] = right.x;
	mat.m[0][1] = right.y;
	mat.m[0][2] = right.z;
	mat.m[0][3] = -Dot(right, _eye);

	mat.m[1][0] = up.x;
	mat.m[1][1] = up.y;
	mat.m[1][2] = up.z;
	mat.m[1][3] = -Dot(up, _eye);

	mat.m[2][0] = forward.x;
	mat.m[2][1] = forward.y;
	mat.m[2][2] = forward.z;
	mat.m[2][3] = -Dot(forward, _eye);

	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;
	mat.m[3][3] = 1.0f;

	return mat;
}

Mat4 Perspective(const float& _fov, const float& _aspectRatio, const float& _near, const float& _far)
{
	Mat4 mat{};

	const float t = 1.0f / std::tan(_fov * 0.5f);

	mat.m[0][0] = t / _aspectRatio;
	mat.m[1][1] = -t;

	mat.m[2][2] = _far / (_far - _near);

	mat.m[2][3] = -(_near * _far / (_far - _near));
	mat.m[3][2] = 1.0f;

	return mat;
}

Mat4 Translate(const Mat4& _mat, const Vec3f& _vec)
{
	Mat4 translate = IdentityMat4();
	translate.m[0][3] = _vec.x;
	translate.m[1][3] = _vec.y;
	translate.m[2][3] = _vec.z;

	return _mat * translate;
}

Mat4 Scale(const Mat4& _mat, const Vec3f& _vec)
{
	Mat4 scale = IdentityMat4();
	scale.m[0][0] = _vec.x;
	scale.m[1][1] = _vec.y;
	scale.m[2][2] = _vec.z;

	return _mat * scale;
}

Mat4 IdentityMat4()
{
	Mat4 mat{};
	mat.m[0][0] = 1.0f;
	mat.m[1][1] = 1.0f;
	mat.m[2][2] = 1.0f;
	mat.m[3][3] = 1.0f;
	return mat;
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

Mat4 TransposeMat4(const Mat4 _m)
{
	Mat4 result{};

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			result.m[x][y] = _m.m[y][x];
		}
	}

	return result;
}

Mat3 TransposeMat3(const Mat3& _mat)
{
	Mat3 mat;
	mat.m[0][0] = _mat.m[0][0];
	mat.m[1][0] = _mat.m[0][1];
	mat.m[2][0] = _mat.m[0][2];

	mat.m[0][1] = _mat.m[1][0];
	mat.m[1][1] = _mat.m[1][1];
	mat.m[2][1] = _mat.m[1][2];

	mat.m[0][2] = _mat.m[2][0];
	mat.m[1][2] = _mat.m[2][1];
	mat.m[2][2] = _mat.m[2][2];

	return mat;
}

float DeterminantMat3(const Mat3& _mat)
{
	return
		_mat.m[0][0] * (_mat.m[1][1] * _mat.m[2][2] -
			_mat.m[1][2] * _mat.m[2][1])

		- _mat.m[0][1] * (_mat.m[1][0] * _mat.m[2][2] -
			_mat.m[1][2] * _mat.m[2][0])

		+ _mat.m[0][2] * (_mat.m[1][0] * _mat.m[2][1] -
			_mat.m[1][1] * _mat.m[2][0]);
}

Mat3 CofactorSignsMat3(const Mat3& _mat)
{
	Mat3 mat;

	mat.m[0][0] = _mat.m[0][0];
	mat.m[1][0] = -_mat.m[1][0];
	mat.m[2][0] = _mat.m[2][0];

	mat.m[0][1] = -_mat.m[0][1];
	mat.m[1][1] = _mat.m[1][1];
	mat.m[2][1] = -_mat.m[2][1];
	
	mat.m[0][2] = _mat.m[0][2];
	mat.m[1][2] = -_mat.m[1][2];
	mat.m[2][2] = _mat.m[2][2];

	return mat;
}

Mat3 MinorMat3(const Mat3& _mat) 
{
	Mat3 mat;

	mat.m[0][0] = _mat.m[1][1] * _mat.m[2][2] - _mat.m[1][2] * _mat.m[2][1];
	mat.m[0][1] = _mat.m[1][0] * _mat.m[2][2] - _mat.m[1][2] * _mat.m[2][0];
	mat.m[0][2] = _mat.m[1][0] * _mat.m[2][1] - _mat.m[1][1] * _mat.m[2][0];

	mat.m[1][0] = _mat.m[0][1] * _mat.m[2][2] - _mat.m[0][2] * _mat.m[2][1];
	mat.m[1][1] = _mat.m[0][0] * _mat.m[2][2] - _mat.m[0][2] * _mat.m[2][0];
	mat.m[1][2] = _mat.m[0][0] * _mat.m[2][1] - _mat.m[0][1] * _mat.m[2][0];

	mat.m[2][0] = _mat.m[0][1] * _mat.m[1][2] - _mat.m[0][2] * _mat.m[1][1];
	mat.m[2][1] = _mat.m[0][0] * _mat.m[1][2] - _mat.m[0][2] * _mat.m[1][0];
	mat.m[2][2] = _mat.m[0][0] * _mat.m[1][1] - _mat.m[0][1] * _mat.m[1][0];

	return mat;
}

Mat3 InverseMat3(const Mat3& _mat)
{
	Mat3 mat;

	const Mat3 minor = MinorMat3(_mat);
	const Mat3 cofactor = TransposeMat3(CofactorSignsMat3(minor));
	const float det =  1 / DeterminantMat3(_mat);

	mat = cofactor * det;

	return mat;
}

Vec3f RotateAroundAxis(Vec3f _v, Vec3f _axis, float _angle)
{
	const float c = cosf(_angle);
	const float s = sinf(_angle);

	return _v * c + Cross(_axis, _v) * s + _axis * Dot(_axis, _v) * (1.0f - c);
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
	return _vA.x + _vB.x + _vA.y * _vB.y;
}

float Angle(const Vec3f& _vA, const Vec3f& _vB)
{
	return acosf(Dot(_vA, _vB) / (Length(_vA) * Length(_vB)));
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

Vec4f DegToRad(const Vec4f& _vec)
{
	return { DegToRad(_vec.x), DegToRad(_vec.y), DegToRad(_vec.z), DegToRad(_vec.w) };
}

Vec3f DegToRad(const Vec3f& _vec)
{
	return { DegToRad(_vec.x), DegToRad(_vec.y), DegToRad(_vec.z) };
}

Vec2f DegToRad(const Vec2f& _vec)
{
	return { DegToRad(_vec.x), DegToRad(_vec.y) };
}

float DegToRad(const float& _a)
{
	return static_cast<float>(_a * (M_PI / 180));
}

Vec4f RadToDeg(const Vec4f& _vec)
{
	return { RadToDeg(_vec.x), RadToDeg(_vec.y), RadToDeg(_vec.z), RadToDeg(_vec.w) };
}

Vec3f RadToDeg(const Vec3f& _vec)
{
	return { RadToDeg(_vec.x), RadToDeg(_vec.y), RadToDeg(_vec.z) };
}

Vec2f RadToDeg(const Vec2f& _vec)
{
	return { RadToDeg(_vec.x), RadToDeg(_vec.y) };
}

float RadToDeg(const float& _a)
{
	return static_cast<float>(_a * (180 / M_PI));
}