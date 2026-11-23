#include "Mat4.h"

#include <intrin.h>

#include "../SSEHelper.h"
#include "../Vector/Vec4.h"

__m128 Mat2Mul(const __m128& _a, const __m128& _b);
__m128 Mat2AdjMul(const __m128& _a, const __m128& _b);
__m128 Mat2MulAdj(const __m128& _a, const __m128& _b);

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
		__m128 r = _mm_mul_ps(SWZ(row, 0, 0, 0, 0), b0);
		r = _mm_add_ps(r, _mm_mul_ps(SWZ(row, 1, 1, 1, 1), b1));
		r = _mm_add_ps(r, _mm_mul_ps(SWZ(row, 2, 2, 2, 2), b2));
		r = _mm_add_ps(r, _mm_mul_ps(SWZ(row, 3, 3, 3, 3), b3));

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

// https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
Mat4 Mat4::Inverse()
{
	const __m128 rowA = _mm_load_ps(m[0]);
	const __m128 rowB = _mm_load_ps(m[1]);
	const __m128 rowC = _mm_load_ps(m[2]);
	const __m128 rowD = _mm_load_ps(m[3]);

	// sub matrices
	const __m128 subA = _mm_movelh_ps(rowA, rowB);
	const __m128 subB = _mm_movehl_ps(rowA, rowB);
	const __m128 subC = _mm_movelh_ps(rowC, rowD);
	const __m128 subD = _mm_movehl_ps(rowC, rowD);

	// determinant as |A| |B| |C| |D|
	const __m128 detSub = _mm_sub_ps(
		_mm_mul_ps(VEC_SHUFFLE(rowA, rowC, 0, 2, 0, 2), VEC_SHUFFLE(rowB, rowD, 1, 3, 1, 3)),
		_mm_mul_ps(VEC_SHUFFLE(rowA, rowC, 1, 3, 1, 3), VEC_SHUFFLE(rowB, rowD, 0, 2, 0, 2))
	);

	const __m128 detA = SWZ_1(detSub, 0);
	const __m128 detB = SWZ_1(detSub, 1);
	const __m128 detC = SWZ_1(detSub, 2);
	const __m128 detD = SWZ_1(detSub, 3);

	const __m128 d_c = Mat2AdjMul(subD, subC);
	const __m128 a_b = Mat2AdjMul(subA, subB);

	__m128 x = _mm_sub_ps(_mm_mul_ps(detD, subA), Mat2Mul(subB, d_c));
	__m128 w = _mm_sub_ps(_mm_mul_ps(detA, subD), Mat2Mul(subC, a_b));

	__m128 detM = _mm_mul_ps(detA, detD);

	__m128 y = _mm_sub_ps(_mm_mul_ps(detB, subC), Mat2MulAdj(subD, a_b));
	__m128 z = _mm_sub_ps(_mm_mul_ps(detC, subB), Mat2MulAdj(subA, d_c));

	detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));

	__m128 tr = _mm_mul_ps(a_b, SWZ(d_c, 0, 2, 1, 3));
	tr = _mm_hadd_ps(tr, tr);
	tr = _mm_hadd_ps(tr, tr);

	detM = _mm_sub_ps(detM, tr);

	const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
	const __m128 rDetM = _mm_div_ps(adjSignMask, detM);

	x = _mm_mul_ps(x, rDetM);
	y = _mm_mul_ps(y, rDetM);
	z = _mm_mul_ps(z, rDetM);
	w = _mm_mul_ps(w, rDetM);

	Mat4 r;
	_mm_store_ps(r.m[0], VEC_SHUFFLE(x, y, 3, 1, 3, 1));
	_mm_store_ps(r.m[1], VEC_SHUFFLE(x, y, 2, 0, 2, 0));
	_mm_store_ps(r.m[2], VEC_SHUFFLE(z, w, 3, 1, 3, 1));
	_mm_store_ps(r.m[3], VEC_SHUFFLE(z, w, 2, 0, 2, 0));

	return r;
}

Mat4 Mat4::Transpose()
{
	const __m128 rowA = _mm_load_ps(m[0]);
	const __m128 rowB = _mm_load_ps(m[1]);
	const __m128 rowC = _mm_load_ps(m[2]);
	const __m128 rowD = _mm_load_ps(m[3]);

	const __m128 t0 = _mm_unpacklo_ps(rowA, rowB);
	const __m128 t1 = _mm_unpacklo_ps(rowC, rowD);
	const __m128 t2 = _mm_unpackhi_ps(rowA, rowB);
	const __m128 t3 = _mm_unpackhi_ps(rowC, rowD);

	Mat4 r;
	_mm_store_ps(r.m[0], _mm_movelh_ps(t0, t1)); 
	_mm_store_ps(r.m[1], _mm_movehl_ps(t1, t0)); 
	_mm_store_ps(r.m[2], _mm_movelh_ps(t2, t3)); 
	_mm_store_ps(r.m[3], _mm_movehl_ps(t3, t2)); 

	return r;
}

__m128 Mat2Mul(const __m128& _a, const __m128& _b)
{
	return _mm_add_ps(
		_mm_mul_ps(_a, SWZ(_b, 0, 3, 0, 3)),
		_mm_mul_ps(SWZ(_a, 1, 0, 3, 2), SWZ(_b, 2, 1, 2, 1))
	);
}

__m128 Mat2AdjMul(const __m128& _a, const __m128& _b)
{
	return _mm_sub_ps(
		_mm_mul_ps(SWZ(_a, 3, 3, 0, 0), _b),
		_mm_mul_ps(SWZ(_a, 1, 1, 2, 2), SWZ(_b, 2, 3, 0, 1))
	);
}

__m128 Mat2MulAdj(const __m128& _a, const __m128& _b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_a, SWZ(_b, 3, 0, 3, 0)),
		_mm_mul_ps(SWZ(_a, 1, 0, 3, 2), SWZ(_b, 2, 1, 2, 1))
	);
}