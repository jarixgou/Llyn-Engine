#ifndef MAT3__H
#define MAT3__H

#include "../../Vector/FwdVec3.h"

struct Mat4;

/*
 *	m00	m01	m02
 *	m10	m11	m12
 *	m20	m21	m22
 */
struct Mat3
{
	float m[3][3] = {
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};

	Mat3& operator*=(const Mat3& _m);
	Mat3 operator*(const Mat3& _other);

	Mat3& operator/=(const Mat3& _other);
	Mat3 operator/(const Mat3& _other);

	bool operator==(const Mat3& _other) const;
	bool operator!=(const Mat3& _other) const;

	Mat3& operator*=(const Vec3f& _vec);
	Mat3 operator*(const Vec3f& _vec) const;

	Mat3 operator*(const float& _f) const;
	Mat3& operator*=(const float& _f);

	Mat3& operator=(const Mat4& _m4);
};

#endif