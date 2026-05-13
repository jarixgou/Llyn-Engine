#ifndef MAT4__H
#define MAT4__H

#include "../../Vector/FwdVec4.h"

struct Mat4
{
	float a0 = 0.0f;
	float a1 = 0.0f;
	float a2 = 0.0f;
	float a3 = 0.0f;

	float b0 = 0.0f;
	float b1 = 0.0f;
	float b2 = 0.0f;
	float b3 = 0.0f;

	float c0 = 0.0f;
	float c1 = 0.0f;
	float c2 = 0.0f;
	float c3 = 0.0f;

	float d0 = 0.0f;
	float d1 = 0.0f;
	float d2 = 0.0f;
	float d3 = 0.0f;

	Mat4& operator*=(const Mat4& _other);
	Mat4 operator*(const Mat4& _other) const;

	bool operator==(const Mat4& _other) const;
	bool operator!=(const Mat4& _other) const;

	Mat4& operator*=(const Vec4f& _vec);
	Mat4 operator*(const Vec4f& _vec) const;
};

#endif