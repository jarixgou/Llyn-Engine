#include "Mat4.h"

#include <immintrin.h>
#include <cstdint>

#include "Mat3.h"
#include "../../Vector/Vec4.h"

Mat4& Mat4::operator*=(const Mat4& _m)
{
	Mat4 result{};

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			result.m[y][x] =
				m[y][0] * _m.m[0][x]
				+ m[y][1] * _m.m[1][x]
				+ m[y][2] * _m.m[2][x]
				+ m[y][3] * _m.m[3][x];
		}
	}

	*this = result;
	return *this;
}


Mat4 Mat4::operator*(const Mat4& _other) const
{
	Mat4 result{};

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			result.m[y][x] =
				m[y][0] * _other.m[0][x]
				+ m[y][1] * _other.m[1][x]
				+ m[y][2] * _other.m[2][x]
				+ m[y][3] * _other.m[3][x];
		}
	}

	return result;
}


bool Mat4::operator==(const Mat4& _other) const
{
	return true;
}

bool Mat4::operator!=(const Mat4& _other) const
{
	return true;
}

Mat4& Mat4::operator*=(const Vec4f& _vec)
{
	Mat4 mat;
	return mat;
}

Mat4 Mat4::operator*(const Vec4f& _vec) const
{
	Mat4 mat;
	return mat;
}

Mat4& Mat4::operator=(const Mat3& _m3)
{
	Mat4 mat = *this;

	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			m[y][x] = _m3.m[y][x];
		}
	}

	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;

	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;

	mat.m[3][3] = 0.0f;

	return mat;
}