#include "Mat3.h"

#include <immintrin.h>

#include "Mat4.h"
#include "../../Vector/Vec3.h"

Mat3& Mat3::operator*=(const Mat3& _m)
{
	Mat3 mat;

#ifdef __AVX__
	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			const __m256 a = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, m[y][0], m[y][1], m[y][2]);
			const __m256 b = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, _m.m[0][x], _m.m[1][x], _m.m[2][x]);

			const __m256 mul = _mm256_mul_ps(a, b);

			__m256 hadd = _mm256_hadd_ps(mul, mul);
			hadd = _mm256_hadd_ps(hadd, hadd);

			alignas(32) float result[8];
			_mm256_store_ps(result, hadd);

			mat.m[y][x] = result[0];
		}
	}
#elif defined(__SSE2__)

#else

#endif

	return mat;
}

Mat3 Mat3::operator*(const Mat3& _other)
{
	Mat3 tmp = *this;
	tmp *= _other;
	return tmp;
}

Mat3& Mat3::operator/=(const Mat3& _other)
{
#ifdef __AVX__
#elif defined(__SSE2__)
#else
#endif

	Mat3 mat;
	return mat;
}

Mat3 Mat3::operator/(const Mat3& _other)
{
	Mat3 mat;
	return mat;
}

bool Mat3::operator==(const Mat3& _other) const
{
	return true;
}

bool Mat3::operator!=(const Mat3& _other) const
{
	return true;
}

Mat3& Mat3::operator*=(const Vec3f& _vec)
{
	Mat3 mat;
	return mat;
}

Mat3 Mat3::operator*(const Vec3f& _vec) const
{
	Mat3 mat;
	return mat;
}

Mat3 Mat3::operator*(const float& _f) const
{
	Mat3 mat;
#ifdef __AVX__
	const __m256 a = _mm256_set_ps(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1]);
	const __m256 b = _mm256_set1_ps(_f);

	alignas(32) float result[8];
	_mm256_store_ps(result, _mm256_mul_ps(a, b));

	mat.m[0][0] = result[7];
	mat.m[0][1] = result[6];
	mat.m[0][2] = result[5];

	mat.m[1][0] = result[4];
	mat.m[1][1] = result[3];
	mat.m[1][2] = result[2];

	mat.m[2][0] = result[1];
	mat.m[2][1] = result[0];
	mat.m[2][2] = m[2][2] * _f;

#elif defined(__SSE2__)
#else

#endif

	return mat;
}

Mat3& Mat3::operator*=(const float& _f)
{
	Mat3 mat = *this * _f;
	return mat;
}

Mat3& Mat3::operator=(const Mat4& _m4)
{
	Mat3 mat;

	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			mat.m[y][x] = _m4.m[y][x];
		}
	}

	return mat;
}
