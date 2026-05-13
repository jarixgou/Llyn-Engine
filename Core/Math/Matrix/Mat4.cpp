#include "Mat4.h"

#include <immintrin.h>
#include <cstdint>

#include "../../Vector/Vec4.h"

float Dot4AVX(const float* _row, const float* _col);
float Dot4SSE(const float* _row, const float* _col);

Mat4& Mat4::operator*=(const Mat4& _other)
{
	const float row[4][4] = {
		{a0, b0, c0, d0},
		{a1, b1, c1, d1},
		{a2, b2, c2, d2},
		{a3, b3, c3, d3}
	};

	const float col[4][4] = {
		{a0, a1, a2, a3},
		{b0, b1, b2, b3},
		{c0, c1, c2, c3},
		{d0, d1, d2, d3}
	};

	float out[4][4] = { 0 };
#ifdef __AVX__
	for (uint8_t y = 0; y < 4; ++y)
	{
		for (uint8_t x = 0; x < 4; ++x)
		{
			out[y][x] = Dot4AVX(row[y], col[x]);
		}
	}
#elif defined(__SSE2__)
	for (uint8_t y = 0; y < 4; ++y)
	{
		for (uint8_t x = 0; x < 4; ++x)
		{
			out[y][x] = Dot4SSE(row[y], col[x]);
		}
	}
#else
	for (size_t y = 0; y < 4; y++)
	{
		for (size_t x = 0; x < 4; x++)
		{
			out[y][x] =
				row[y][0] * col[x][0] +
				row[y][1] * col[x][1] +
				row[y][2] * col[x][2] + 
				row[y][3] * col[x][3];
		}
	}
#endif

	a0 = out[0][0]; a1 = out[0][1]; a2 = out[0][2]; a3 = out[0][3];
	b0 = out[1][0]; b1 = out[1][1]; b2 = out[1][2]; b3 = out[1][3];
	c0 = out[2][0]; c1 = out[2][1]; c2 = out[2][2]; c3 = out[2][3];
	d0 = out[3][0]; d1 = out[3][1]; d2 = out[3][2]; d3 = out[3][3];

	return *this;
}

Mat4 Mat4::operator*(const Mat4& _other) const
{
	Mat4 tmp = *this;
	tmp *= _other;
	return tmp;
}

bool Mat4::operator==(const Mat4& _other) const
{}

bool Mat4::operator!=(const Mat4& _other) const
{}

Mat4& Mat4::operator*=(const Vec4f& _vec)
{}

Mat4 Mat4::operator*(const Vec4f& _vec) const
{}

float Dot4AVX(const float* _row, const float* _col)
{
	const __m256 r = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, _row[3], _row[2], _row[1], _row[0]);
	const __m256 c = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, _col[3], _col[2], _col[1], _col[0]);

	const __m256 mul = _mm256_mul_ps(r, c);
	const __m256 sum1 = _mm256_hadd_ps(mul, mul);
	const __m256 sum2 = _mm256_hadd_ps(sum1, sum1);

	alignas(32) float result[8];
	_mm256_store_ps(result, sum2);

	return result[0];
}

float Dot4SSE(const float* _row, const float* _col)
{
	const __m128 r = _mm_set_ps(_row[3], _row[2], _row[1], _row[0]);
	const __m128 c = _mm_set_ps(_col[3], _col[2], _col[1], _col[0]);

	const __m128 mul = _mm_mul_ps(r, c);
	__m128 shuf = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
	__m128 sum = _mm_add_ps(mul, shuf);
	shuf = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(1, 0, 3, 2));
	sum = _mm_add_ps(sum, shuf);

	return _mm_cvtss_f32(sum);
}