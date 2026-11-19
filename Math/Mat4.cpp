#include "Mat4.h"

#include <intrin.h>

#include "SSEHelper.h"
#include "Vector/Vec4.h"

Mat4::Mat4()
{
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			m[y][x] = 0.0f;
		}
	}
}

Mat4& Mat4::operator*=(const Mat4& _a)
{
	const __m128 b0 = _mm_load_ps(_a.m[0]);
	const __m128 b1 = _mm_load_ps(_a.m[1]);
	const __m128 b2 = _mm_load_ps(_a.m[2]);
	const __m128 b3 = _mm_load_ps(_a.m[3]);

	for (int i = 0; i < 4; ++i)
	{
		const __m128 row = _mm_load_ps(m[i]);
		__m128 r = _mm_mul_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(0, 0, 0, 0)), b0);
		r = _mm_add_ps(r, _mm_mul_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(1, 1, 1, 1)), b1));
		r = _mm_add_ps(r, _mm_mul_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(2, 2, 2, 2)), b2));
		r = _mm_add_ps(r, _mm_mul_ps(_mm_shuffle_ps(row, row, _MM_SHUFFLE(3, 3, 3, 3)), b3));

		_mm_store_ps(m[i], r);
	}

	return *this;
}

Mat4 Mat4::operator*(const Mat4& _a)
{
	Mat4 tmp = *this;
	tmp *= _a;

	return tmp;
}

bool Mat4::operator==(const Mat4& _a) const
{
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			if (m[row][col] < _a.m[row][col] || m[row][col] > _a.m[row][col])
			{
				return false;
			}
		}
	}

	return true;
}

bool Mat4::operator!=(const Mat4& _a) const
{
	return !(*this == _a);
}

Mat4& Mat4::operator*=(const float& _a)
{
	const __m128 scalar = _mm_set1_ps(_a);

	for (int i = 0; i < 4; ++i)
	{
		const __m128 row = _mm_load_ps(m[i]);

		const __m128 mul = _mm_mul_ps(row, scalar);

		_mm_store_ps(m[i], mul);
	}

	return *this;
}

Mat4 Mat4::operator*(const float& _a)
{
	Mat4 tmp = *this;
	tmp *= _a;

	return tmp;
}

float Mat4::Determinant()
{
	const __m128 rowA = _mm_load_ps(m[0]);
	const __m128 rowB = _mm_load_ps(m[1]);
	const __m128 rowC = _mm_load_ps(m[2]);
	const __m128 rowD = _mm_load_ps(m[3]);

	// sub matrix det
	const __m128 sA = _mm_sub_ps(
		_mm_mul_ps(SWZ(rowA, 0, 0, 0, 1), SWZ(rowB, 1, 2, 3, 2)),
		_mm_mul_ps(SWZ(rowA, 1, 2, 3, 2), SWZ(rowB, 0, 0, 0, 1))
		);

	const __m128 sB = _mm_sub_ps(
		_mm_mul_ps(SWZ(rowA, 1, 2, 0, 0), SWZ(rowB, 3, 3, 0, 0)),
		_mm_mul_ps(SWZ(rowA, 3, 3, 0, 0), SWZ(rowB, 1, 2, 0, 0))
	);

	const __m128 cA = _mm_sub_ps(
		_mm_mul_ps(SWZ(rowC, 2, 1, 1, 0), SWZ(rowB, 3, 3, 2, 3)),
		_mm_mul_ps(SWZ(rowC, 3, 3, 2, 3), SWZ(rowD, 2, 1, 1, 0))
	);

	const __m128 cB = _mm_sub_ps(
		_mm_mul_ps(SWZ(rowC, 0, 0, 0, 0), SWZ(rowD, 2, 1, 0, 0)),
		_mm_mul_ps(SWZ(rowC, 2, 1, 0, 0), SWZ(rowD, 0, 0, 0, 0))
		);

	const __m128 pA = _mm_mul_ps(_mm_mul_ps(sA, cA), _mm_setr_ps(1.f, -1.f, 1.f, 1.f));
	const __m128 pB = _mm_mul_ps(_mm_mul_ps(sB, cB), _mm_setr_ps(-1.f, 1.f, 0.f, 0.f));

	__m128 s = _mm_add_ps(pA, pB);
	s = _mm_add_ps(s, _mm_movehl_ps(s, s));
	s = _mm_add_ss(s, SWZ(s, 1, 1, 1, 1));
	return _mm_cvtss_f32(s);
}
