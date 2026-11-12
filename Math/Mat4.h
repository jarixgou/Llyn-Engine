#ifndef MAT_4__H
#define MAT_4__H

#include "Vector/FwdVec4.h"

struct Mat4
{
	alignas(16) float m[4][4];

	Mat4();

	Mat4& operator*=(const Mat4& _a);
	Mat4 operator*(const Mat4& _a);

	bool operator==(const Mat4& _a) const;
	bool operator!=(const Mat4& _a) const;

	Mat4& operator*=(const float& _a);
	Mat4 operator*(const float& _a);

	float Determinant();
};

#endif