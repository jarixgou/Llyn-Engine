#include "Mat3.h"

#include <immintrin.h>

#include "../Vector/Vec3.h"

Mat3& Mat3::operator+=(const Mat3& _other)
{
#ifdef __AVX__
	const __m256 row = _mm256_set_ps(0.f, 0.f, b2, b1, b0, a2, a1, a0);
	const __m256 row1 = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, c2, c1, c0);

	const __m256 col = _mm256_set_ps(0.f, 0.f, _other.b2, _other.b1, _other.b0, _other.a2, _other.a1, _other.a0);
	const __m256 col1 = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _other.c2, _other.c1, _other.c0);

	const __m256 add = _mm256_add_ps(row, col);
	const __m256 add1 = _mm256_add_ps(row1, col1);

	alignas(32) float result[8];
	alignas(32) float result1[8];
	_mm256_store_ps(result, add);
	_mm256_store_ps(result1, add1);

	a0 = result[0]; a1 = result[1]; a2 = result[2];
	b0 = result[3]; b1 = result[4]; b2 = result[5];
	c0 = result1[0]; c1 = result1[1]; c2 = result1[2];
#elif defined(__SSE2__)
	__m128 row = _mm_set_ps(0.0f, a2, a1, a0);
	__m128 row1 = _mm_set_ps(0.0f, b2, b1, b0);
	__m128 row2 = _mm_set_ps(0.0f, c2, c1, c0);

	__m128 col = _mm_set_ps(0.0f, _other.a2, _other.a1, _other.a0);
	__m128 col1 = _mm_set_ps(0.0f, _other.b2, _other.b1, _other.b0);
	__m128 col2 = _mm_set_ps(0.0f, _other.c2, _other.c1, _other.c0);

	__m128 add = _mm_add_ps(row, col);
	__m128 add1 = _mm_add_ps(row1, col1);
	__m128 add2 = _mm_add_ps(row2, col2);

	alignas(16) float result[4];
	alignas(16) float result1[4];
	alignas(16) float result2[4];
	_mm_store_ps(result, add);
	_mm_store_ps(result1, add1);
	_mm_store_ps(result2, add2);

	a0 = result[0]; a1 = result[1]; a2 = result[2];
	b0 = result1[0]; b1 = result1[1]; b2 = result1[2];
	c0 = result2[0]; c1 = result2[1]; c2 = result2[2];
#else
	a0 += _other.a0; a1 += _other.a1; a2 += _other.a2;
	b0 += _other.b0; b1 += _other.b1; b2 += _other.b2;
	c0 += _other.c0; c1 += _other.c1; c2 += _other.c2;
#endif

	return *this;
}

Mat3 Mat3::operator+(const Mat3& _other) const
{
	Mat3 tmp = *this;
	tmp += _other;
	return tmp;
}

Mat3& Mat3::operator-=(const Mat3& _other)
{
#ifdef __AVX__
	const __m256 row = _mm256_set_ps(0.f, 0.f, b2, b1, b0, a2, a1, a0);
	const __m256 row1 = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, c2, c1, c0);

	const __m256 col = _mm256_set_ps(0.f, 0.f, _other.b2, _other.b1, _other.b0, _other.a2, _other.a1, _other.a0);
	const __m256 col1 = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _other.c2, _other.c1, _other.c0);

	const __m256 add = _mm256_sub_ps(row, col);
	const __m256 add1 = _mm256_sub_ps(row1, col1);

	alignas(32) float result[8];
	alignas(32) float result1[8];
	_mm256_store_ps(result, add);
	_mm256_store_ps(result1, add1);

	a0 = result[0]; a1 = result[1]; a2 = result[2];
	b0 = result[3]; b1 = result[4]; b2 = result[5];
	c0 = result1[0]; c1 = result1[1]; c2 = result1[2];
#elif defined(__SSE2__)
	__m128 row = _mm_set_ps(0.0f, a2, a1, a0);
	__m128 row1 = _mm_set_ps(0.0f, b2, b1, b0);
	__m128 row2 = _mm_set_ps(0.0f, c2, c1, c0);

	__m128 col = _mm_set_ps(0.0f, _other.a2, _other.a1, _other.a0);
	__m128 col1 = _mm_set_ps(0.0f, _other.b2, _other.b1, _other.b0);
	__m128 col2 = _mm_set_ps(0.0f, _other.c2, _other.c1, _other.c0);

	__m128 add = _mm_sub_ps(row, col);
	__m128 add1 = _mm_sub_ps(row1, col1);
	__m128 add2 = _mm_sub_ps(row2, col2);

	alignas(16) float result[4];
	alignas(16) float result1[4];
	alignas(16) float result2[4];
	_mm_store_ps(result, add);
	_mm_store_ps(result1, add1);
	_mm_store_ps(result2, add2);

	a0 = result[0]; a1 = result[1]; a2 = result[2];
	b0 = result1[0]; b1 = result1[1]; b2 = result1[2];
	c0 = result2[0]; c1 = result2[1]; c2 = result2[2];
#else
	a0 -= _other.a0; a1 -= _other.a1; a2 -= _other.a2;
	b0 -= _other.b0; b1 -= _other.b1; b2 -= _other.b2;
	c0 -= _other.c0; c1 -= _other.c1; c2 -= _other.c2;
#endif
	
	return *this;
}

Mat3 Mat3::operator-(const Mat3& _other)
{
	Mat3 tmp = *this;
	tmp -= _other;
	return tmp;
}

Mat3& Mat3::operator*=(const Mat3& _other)
{
#ifdef __AVX__
	const __m256 row = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, a2, a1, a0);
	const __m256 row1 = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, b2, b1, b0);
	const __m256 row2 = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, c2, c1, c0);

	const __m256 col = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, _other.c0, _other.b0, _other.a0);
	const __m256 col1 = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, _other.c1, _other.b1, _other.a1);
	const __m256 col2 = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, _other.c2, _other.b2, _other.a2);

	const __m256 mul = _mm256_mul_ps(row, col);
	const __m256 mul1 = _mm256_mul_ps(row1, col1);
	const __m256 mul2 = _mm256_mul_ps(row2, col2);

#elif defined(__SSE2__)
#else
	
#endif
}

Mat3 Mat3::operator*(const Mat3& _other)
{
}

Mat3& Mat3::operator/=(const Mat3& _other)
{
#ifdef __AVX__
#elif defined(__SSE2__)
#else
	a0 += _other.a0; a1 += _other.a1; a2 += _other.a2;
	b0 += _other.b0; b1 += _other.b1; b2 += _other.b2;
	c0 += _other.c0; c1 += _other.c1; c2 += _other.c2;
#endif
}

Mat3 Mat3::operator/(const Mat3& _other)
{
}

bool Mat3::operator==(const Mat3& _other) const
{
}

bool Mat3::operator!=(const Mat3& _other) const
{
}

Mat3& Mat3::operator*=(const Vec3f& _vec)
{

}

Mat3 Mat3::operator*(const Vec3f& _vec) const
{

}
