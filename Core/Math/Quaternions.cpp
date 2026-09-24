#include "Quaternions.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "Math.h"
#include "../Vector/Vec3.h"
#include "Matrix/Mat4.h"

void Quaternions::Normalize()
{
	float magnitude = sqrtf(w * w + x * x + y * y + z * z);
	w /= magnitude;
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
}

Mat4 Quaternions::GetMatrix() const
{
	Mat4 mat;

	const float xSqr = x * x;
	const float ySqr = y * y;
	const float zSqr = z * z;

	const float xy = x * y;
	const float xz = x * z;
	const float yz = y * z;

	const float wx = w * x;
	const float wy = w * y;
	const float wz = w * z;

	mat.m[0][0] = 1.0f - 2.0f * (ySqr + zSqr);
	mat.m[0][1] = 2.0f * (xy + wz);
	mat.m[0][2] = 2.0f * (xz - wy);
	mat.m[0][3] = 0.0f;

	mat.m[1][0] = 2.0f * (xy - wz);
	mat.m[1][1] = 1.0f - 2.0f * (xSqr + zSqr);
	mat.m[1][2] = 2.0f * (yz + wx);
	mat.m[1][3] = 0.0f;

	mat.m[2][0] = 2.0f * (xz + wy);
	mat.m[2][1] = 2.0f * (yz - wx);
	mat.m[2][2] = 1.0f - 2.0f * (xSqr + ySqr);
	mat.m[2][3] = 0.0f;

	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;
	mat.m[3][3] = 1.0f;

	return mat;
}


void Quaternions::SetEuler(Vec3f _vec)
{
	const float cosX = cosf(_vec.x / 2);
	const float cosY = cosf(_vec.y / 2);
	const float cosZ = cosf(_vec.z / 2);

	const float sinX = sinf(_vec.x / 2);
	const float sinY = sinf(_vec.y / 2);
	const float sinZ = sinf(_vec.z / 2);

	w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	x = sinX * cosY * cosZ - cosX * sinY * sinZ;
	y = cosX * sinY * cosZ + sinX * cosY * sinZ;
	z = cosX * cosY * sinZ - sinX * sinY * cosZ;
}

Vec3f Quaternions::GetEuler() const
{
	Vec3f vec{};

	vec.x = atan2f(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));
	vec.y = -M_PI / 2 + 2.0f *
		atan2f(sqrtf(1.0f + 2.0f * (w * y - x * z)), sqrtf(1.0f - 2.0f * (w * y - x * z)));
	vec.z = atan2f(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));

	return vec;
}

void Quaternions::SetAxisAngle(const Vec3f& _axis, float _angle)
{
	const float c = cosf(_angle / 2);
	const float s = sinf(_angle / 2);

	w = c;
	x = _axis.x * s;
	y = _axis.y * s;
	z = _axis.z * s;
}

Vec3f Quaternions::Rotate(const Vec3f& _v)
{
	Vec3f t = Cross({ x, y, z }, _v) * 2.0f;

	return _v + t * w + Cross({ x, y, z }, t);
}

Vec3f Quaternions::GetForward() const
{
	Vec3f vec;
	vec.x = 2.0f * (x * z + w * y);
	vec.y = 2.0f * (y * z - w * x);
	vec.z = 1.0f - 2.0f * (x * x + y * y);

	return vec;
}

Vec3f Quaternions::GetRight()
{
	Vec3f vec;
	vec.x = 1.0f - 2.0f * (y * y + z * z);
	vec.y = 2.0f * (x * y + w * z);
	vec.z = 2.0f * (x * z - w * y);
	
	return vec;
}

Vec3f Quaternions::GetUp()
{
	Vec3f vec;
	vec.x = 2.0f * (x * y - w * z);
	vec.y = 1.0f - 2.0f * (x * x + z * z);
	vec.z = 2.0f * (y * z + w * x);

	return vec;
}

Quaternions Quaternions::operator*(const Quaternions& _other)
{
	Quaternions quat;
	quat.w = w * _other.w - x * _other.x - y * _other.y - z * _other.z;
	quat.x = w * _other.x + x * _other.w + y * _other.z - z * _other.y;
	quat.y = w * _other.y - x * _other.z + y * _other.w + z * _other.x;
	quat.z = w * _other.z + x * _other.y - y * _other.x + z * _other.w;

	return quat;
}

Quaternions& Quaternions::operator*=(const Quaternions& _other)
{
	*this = *this * _other;
	return *this;
}
