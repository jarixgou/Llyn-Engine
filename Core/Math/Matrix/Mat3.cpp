#include "Mat3.h"

#include <immintrin.h>

#include "../../Vector/Vec3.h"

float Dot3AVX(const float* _row, const float* _col);
float Dot3SSE(const float* _row, const float* _col);

Mat3& Mat3::operator*=(const Mat3& _other)
{
	const float row[3][3] = {
		{a0, a1, a2},
		{b0, b1, b2},
		{c0, c1, c2}
	};

	const float col[3][3] = {
		{ _other.a0, _other.b0, _other.c0 },
		{ _other.a1, _other.b1, _other.c1 },
		{ _other.a2, _other.b2, _other.c2 }
	};

	float out[3][3] = { 0 };

#ifdef __AVX__
	for (size_t y = 0; y < 3; y++)
	{
		for (size_t x = 0; x < 3; x++)
		{
			out[y][x] = Dot3AVX(row[y], col[x]);
		}
	}
#elif defined(__SSE2__)
	for (size_t y = 0; y < 3; y++)
	{
		for (size_t x = 0; x < 3; x++)
		{
			out[y][x] = Dot3SSE(row[y], col[x]);
		}
	}
#else
	for (size_t y = 0; y < 3; y++)
	{
		for (size_t x = 0; x < 3; x++)
		{
			out[y][x] = 
				row[y][0] * col[x][0] + 
				row[y][1] * col[x][1] +
				row[y][2] * col[x][2];
		}
	}
#endif

	a0 = out[0][0]; a1 = out[0][1]; a2 = out[0][2];
	b0 = out[1][0]; b1 = out[1][1]; b2 = out[1][2];
	c0 = out[2][0]; c1 = out[2][1]; c2 = out[2][2];

	return *this;
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
	a0 += _other.a0; a1 += _other.a1; a2 += _other.a2;
	b0 += _other.b0; b1 += _other.b1; b2 += _other.b2;
	c0 += _other.c0; c1 += _other.c1; c2 += _other.c2;
#endif
}

Mat3 Mat3::operator/(const Mat3& _other)
{}

bool Mat3::operator==(const Mat3& _other) const
{}

bool Mat3::operator!=(const Mat3& _other) const
{}

Mat3& Mat3::operator*=(const Vec3f& _vec)
{

}

Mat3 Mat3::operator*(const Vec3f& _vec) const
{

}

float Dot3AVX(const float* _row, const float* _col)
{
	const __m256 r = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, _row[2], _row[1], _row[0]);
	const __m256 c = _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, _col[2], _col[1], _col[0]);

	const __m256 mul = _mm256_mul_ps(r, c);
	const __m256 sum1 = _mm256_hadd_ps(mul, mul);
	const __m256 sum2 = _mm256_hadd_ps(sum1, sum1);

	alignas(32) float result[8];
	_mm256_store_ps(result, sum2);

	return result[0];
}

float Dot3SSE(const float* _row, const float* _col)
{
	const __m128 r = _mm_set_ps(0.0f, _row[2], _row[1], _row[0]);
	const __m128 c = _mm_set_ps(0.0f, _col[2], _col[1], _col[0]);

	const __m128 mul = _mm_mul_ps(r, c);
	__m128 shuf = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
	__m128 sum = _mm_add_ps(mul, shuf);
	shuf = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(1, 0, 3, 2));
	sum = _mm_add_ps(sum, shuf);

	return _mm_cvtss_f32(sum);
}
