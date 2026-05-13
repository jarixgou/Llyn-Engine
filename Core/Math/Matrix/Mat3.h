#ifndef MAT3__H
#define MAT3__H

#include "../../Vector/FwdVec3.h"

struct Mat3
{
	float a0 = 0;
	float a1 = 0;
	float a2 = 0;

	float b0 = 0;
	float b1 = 0;
	float b2 = 0;

	float c0 = 0;
	float c1 = 0;
	float c2 = 0;

	Mat3& operator*=(const Mat3& _other);
	Mat3 operator*(const Mat3& _other);

	Mat3& operator/=(const Mat3& _other);
	Mat3 operator/(const Mat3& _other);

	bool operator==(const Mat3& _other) const;
	bool operator!=(const Mat3& _other) const;

	Mat3& operator*=(const Vec3f& _vec);
	Mat3 operator*(const Vec3f& _vec) const;
};

#endif