#include "Quaternions.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "../Vector/Vec3.h"
#include "Matrix/Mat4.h"

Mat4 Quaternions::GetMatrix() const
{
	Mat4 mat;

	const float xSqr = x * x;
	const float ySqr = y * y;
	const float zSqr = z * z;
	const float xy = x * y;
	const float wz = w * z;
	const float xz = x * z;
	const float wy = w * y;
	const float yz = y * z;
	const float wx = w * x;

	mat.a0 = 1.0f - 2.0f * (ySqr + zSqr);
	mat.a1 = 2.0f * (xy + wz);
	mat.a2 = 2.0f * (xz - wy);
	mat.a3 = 0.0f;

	mat.b0 = 2.0f * (xy - wz);
	mat.b1 = 1.0f - 2.0f * (xSqr + zSqr);
	mat.b2 = 2.0f * (yz + wx);
	mat.b3 = 0.0f;

	mat.c0 = 2.0f * (xz + wy);
	mat.c1 = 2.0f * (yz - wx);
	mat.c2 = 1 - 2.0f * (xSqr + ySqr);
	mat.c3 = 0.0f;

	mat.d0 = 0.0f;
	mat.d1 = 0.0f;
	mat.d2 = 0.0f;
	mat.d3 = 1.0f;

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
