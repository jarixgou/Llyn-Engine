#include "Math.h"

#include <corecrt_math_defines.h>
#include <intrin.h>

#include "Vector/Vec4.h"
#include "Vector/Vec3.h"

constexpr float degRad = static_cast<float>(M_PI / 180.0f);
constexpr float radDeg = static_cast<float>(180.0f/ M_PI);

Vec3f DegToRad(const Vec3f& _v)
{
	return { _v.x * degRad, _v.y * degRad, _v.z * degRad };
}

Vec3f RadToDeg(const Vec3f& _v)
{
	return { _v.x * radDeg, _v.y * radDeg, _v.z * radDeg };
}

Vec4f DegToRad(const Vec4f& _v)
{
	const __m128 v = _mm_load_ps(&_v.x);
	const __m128 r = _mm_set1_ps(degRad);

	const __m128 mul = _mm_mul_ps(v, r);

	Vec4f result{};
	_mm_store_ps(&result.x, mul);

	return result;
}

Vec4f RadToDeg(const Vec4f& _v)
{
	const __m128 v = _mm_load_ps(&_v.x);
	const __m128 r = _mm_set1_ps(radDeg);

	const __m128 mul = _mm_mul_ps(v, r);

	Vec4f result{};
	_mm_store_ps(&result.x, mul);

	return result;
}