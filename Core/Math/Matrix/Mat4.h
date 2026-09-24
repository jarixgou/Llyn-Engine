#ifndef MAT4__H
#define MAT4__H

#include "../../Vector/FwdVec4.h"

struct Mat3;

/*
 *	m00	 m01  m02  m03
 *	m10	 m11  m12  m13
 *	m20	 m21  m22  m23
 *	m30	 m31  m32  m33
 */
struct Mat4
{
	float m[4][4] = {
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	Mat4& operator*=(const Mat4& _m);
	Mat4 operator*(const Mat4& _other) const;

	bool operator==(const Mat4& _other) const;
	bool operator!=(const Mat4& _other) const;

	Mat4& operator*=(const Vec4f& _vec);
	Mat4 operator*(const Vec4f& _vec) const;

	Mat4& operator=(const Mat3& _m3);
};

#endif