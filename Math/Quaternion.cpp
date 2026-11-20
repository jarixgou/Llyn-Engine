#include "Quaternion.h"

#include <intrin.h>

#include "Vector/Vec3.h"

#include "SSEHelper.h"

Quaternion::Quaternion()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Quaternion::Quaternion(const float& _x, const float& _y, const float& _z)
{
	w = 1.0f;
	x = _x;
	y = _y;
	z = _z;
}

Quaternion::Quaternion(const float& _w, const float& _x, const float& _y, const float& _z)
{
	w = _w;
	x = _x;
	y = _y;
	z = _z;
}

Quaternion& Quaternion::operator*=(const Quaternion& _a)
{
	const __m128 qA = _mm_load_ps(&w);
	const __m128 qB = _mm_load_ps(&_a.w);

	const __m128 t0 = _mm_mul_ps(SWZ(qA, 0, 0, 0, 0), qB);
	const __m128 t1 = _mm_mul_ps(SWZ(qA, 1, 1, 1, 1), SWZ(qB, 1, 0, 3, 2));
	const __m128 t2 = _mm_mul_ps(SWZ(qA, 2, 2, 2, 2), SWZ(qB, 2, 3, 0, 1));
	const __m128 t3 = _mm_mul_ps(SWZ(qA, 3, 3, 3, 3), SWZ(qB, 3, 2, 1, 0));

	const __m128 t1Sign = _mm_xor_ps(t1, _mm_setr_ps(-0.0f, 0.0f, -0.0f, 0.0f));
	const __m128 t2Sign = _mm_xor_ps(t2, _mm_setr_ps(-0.0f, 0.0f, 0.0f, -0.0f));
	const __m128 t3Sign = _mm_xor_ps(t3, _mm_setr_ps(-0.0f, -0.0f, 0.0f, 0.0f));

	_mm_store_ps(&w, _mm_add_ps(_mm_add_ps(t0, t1Sign), _mm_add_ps(t2Sign, t3Sign)));

	return *this;
}

Quaternion Quaternion::operator*(const Quaternion& _a)
{
	Quaternion tmp = *this;
	tmp *= _a;
	return tmp;
}

bool Quaternion::operator==(const Quaternion& _a) const
{
	return !(w > _a.w || w < _a.w) && !(x > _a.x || x < _a.x) && !(y > _a.y || y < _a.y) && !(z > _a.z || z < _a.z);
}

bool Quaternion::operator!=(const Quaternion& _a) const
{
	return !(*this == _a);
}

Vec3f Quaternion::GetEuler()
{

}

Vec3f Quaternion::SetEuler()
{
}

Quaternion Quaternion::Normalize()
{
}

Quaternion Quaternion::Identity()
{
}

void Quaternion::SetFromToRotation(const Vec3f& _fromDirection, const Vec3f& _toDirection)
{
}

void Quaternion::SetLookRotation(const Vec3f& _view, const Vec3f& _up)
{
}

void Quaternion::ToAngleAxis(const float& _angle, const Vec3f& _axis)
{
}

float Quaternion::Angle(const Quaternion& _a, const Quaternion& _b)
{
}

Quaternion Quaternion::AngleAxis(const float& _angle, const Vec3f& _axis)
{
}

float Quaternion::Dot(const Quaternion& _a, const Quaternion& _b)
{
}

Quaternion Quaternion::Euler(const Vec3f& _angle)
{
}

Quaternion Quaternion::FromToRotation(const Vec3f& _fromDirection, const Vec3f& _toDirection)
{
}

Quaternion Quaternion::Inverse(const Quaternion& _a)
{
}

Quaternion Quaternion::Lerp(const Quaternion& _a, const Quaternion& _b, const float& _t)
{
}

Quaternion Quaternion::LookRotation(const Vec3f& _view, const Vec3f& _up)
{
}

Quaternion Quaternion::RotateTowards(const Quaternion& _from, const Quaternion& _to, const float& _maxDegreesDelta)
{
}

Quaternion Quaternion::Slerp(const Quaternion& _a, const Quaternion& _b, const float& _t)
{
}
