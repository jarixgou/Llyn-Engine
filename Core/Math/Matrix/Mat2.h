#ifndef MAT2__H
#define MAT2__H

#include "../../Vector/FwdVec2.h"

/*
 *	m00	m01
 *	m10	m11
 */
struct Mat2
{
	float a0 = 0.0f;
	float a1 = 0.0f;

	float b0 = 0.0f;
	float b1 = 0.0f;

	Mat2& operator+=(const Mat2& _other);
	Mat2 operator+(const Mat2& _other);

	Mat2& operator-=(const Mat2& _other);
	Mat2 operator-(const Mat2& _other) const;

	Mat2& operator*=(const Mat2& _other);
	Mat2 operator*(const Mat2& _other) const;

	Mat2& operator/=(const Mat2& _other);
	Mat2 operator/(const Mat2& _other) const;

	bool operator==(const Mat2& _other) const;
	bool operator!=(const Mat2& _other) const;

	Mat2& operator*=(const Vec2f& _vec);
	Mat2 operator*(const Vec2f& _vec) const;
};

#endif