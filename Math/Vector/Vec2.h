#ifndef VEC_2__H
#define VEC_2__H
#include <cmath>
#include <emmintrin.h>
#include <xmmintrin.h>

template<typename T>
class alignas(16) Vec2
{
public:
	T x;
	T y;

	T padding[2];
public:
	Vec2();
	Vec2(T _x, T _y);

	Vec2& operator+=(const Vec2& _a);
	Vec2 operator+(const Vec2& _a);

	Vec2& operator-=(const Vec2& _a);
	Vec2 operator-(const Vec2& _a);

	Vec2& operator*=(const Vec2& _a);
	Vec2 operator*(const Vec2& _a);

	Vec2& operator/=(const Vec2& _a);
	Vec2 operator/(const Vec2& _a);

	Vec2& operator*=(const T& _a);
	Vec2 operator*(const T& _a);

	Vec2& operator/=(const T& _a);
	Vec2 operator/(const T& _a);

	bool operator==(const Vec2& _a) const;
	bool operator!=(const Vec2& _a) const;

	static Vec2 One();
	static Vec2 Zero();

	static Vec2 Up();
	static Vec2 Down();
	static Vec2 Right();
	static Vec2 Left();

	Vec2<float> Normalize();
	float Length();
	float LengthSqr();

	static float Distance(Vec2 _a, Vec2 _b);
	static float Dot(Vec2 _a, Vec2 _b);
	static float Angle(Vec2 _a, Vec2 _b);
};

template<>
class alignas(16) Vec2<float>
{
public:
	float x = 0.0f;
	float y = 0.0f;

	float padding[2] = { 0.0f };
public:
	Vec2()
	{
		x = 0.0f;
		y = 0.0f;
	}

	Vec2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	Vec2& operator+=(const Vec2& _a)
	{
		const __m128 a = _mm_load_ps(&x);
		const __m128 b = _mm_load_ps(&_a.x);

		const __m128 add = _mm_add_ps(a, b);

		_mm_store_ps(&x, add);

		return *this;
	}
	Vec2 operator+(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp += _a;

		return tmp;
	}

	Vec2& operator-=(const Vec2& _a)
	{
		const __m128 a = _mm_load_ps(&x);
		const __m128 b = _mm_load_ps(&_a.x);

		const __m128 sub = _mm_add_ps(a, b);

		_mm_store_ps(&x, sub);

		return *this;
	}
	Vec2 operator-(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp -= _a;

		return tmp;
	}

	Vec2& operator*=(const Vec2& _a)
	{
		const __m128 a = _mm_load_ps(&x);
		const __m128 b = _mm_load_ps(&_a.x);

		const __m128 mul = _mm_mul_ps(a, b);

		_mm_store_ps(&x, mul);

		return *this;
	}
	Vec2 operator*(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec2& operator/=(const Vec2& _a)
	{
		const __m128 a = _mm_load_ps(&x);
		const __m128 b = _mm_load_ps(&_a.x);

		const __m128 div = _mm_div_ps(a, b);

		_mm_store_ps(&x, div);

		return *this;
	}
	Vec2 operator/(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	Vec2& operator*=(const float& _a)
	{
		const Vec2 other = { _a, _a };

		const __m128 a = _mm_load_ps(&x);
		const __m128 b = _mm_load_ps(&other.x);

		const __m128 mul = _mm_mul_ps(a, b);

		_mm_store_ps(&x, mul);

		return *this;
	}
	Vec2 operator*(const float& _a)
	{
		Vec2 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec2& operator/=(const float& _a)
	{
		const Vec2 other = { _a, _a };

		const __m128 a = _mm_load_ps(&x);
		const __m128 b = _mm_load_ps(&other.x);

		const __m128 div = _mm_div_ps(a, b);

		_mm_store_ps(&x, div);

		return *this;
	}
	Vec2 operator/(const float& _a)
	{
		Vec2 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	bool operator==(const Vec2& _a) const
	{
		return x == _a.x && y == _a.y;
	}
	bool operator!=(const Vec2& _a) const
	{
		return !(*this == _a);
	}

	static Vec2 One()
	{
		return {1.0f, 1.0f};
	}

	static Vec2 Zero()
	{
		return {0.0f, 0.0f};
	}

	static Vec2 Up()
	{
		return {0.0f, 1.0f};
	}

	static Vec2 Down()
	{
		return {0.0f, -1.0f};
	}

	static Vec2 Right()
	{
		return {1.0f, 0.0f};
	}

	static Vec2 Left()
	{
		return {-1.0f, 0.0f};
	}

	Vec2<float> Normalize()
	{
		const float l = Length();
		const Vec2 length = { l, l };

		const __m128 num = _mm_load_ps(&x);
		const __m128 denom = _mm_load_ps(&length.x);

		const __m128 magn = _mm_div_ps(num, denom);

		Vec2 value;
		_mm_store_ps(&value.x, magn);

		return value;
	}

	float Length()
	{
		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = a;

		const __m128 pow = _mm_mul_ps(a, b);

		const __m128 shift = _mm_shuffle_ps(pow, pow, _MM_SHUFFLE(1, 1, 1, 1));

		const __m128 sum = _mm_add_ps(pow, shift);

		const __m128 root = _mm_sqrt_ps(sum);

		return _mm_cvtss_f32(root);
	}

	float LengthSqr()
	{
		float r = 0.0f;

		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = a;

		const __m128 pow = _mm_mul_ps(a, b);

		const __m128 shift = _mm_shuffle_ps(pow, pow, _MM_SHUFFLE(1, 1, 1, 1));

		const __m128 sum = _mm_add_ps(pow, shift);

		r = _mm_cvtss_f32(sum);

		return r;
	}

	static float Distance(Vec2 _a, Vec2 _b)
	{
		Vec2 l = _a - _b;

		return l.Length();
	}

	static float Dot(Vec2 _a, Vec2 _b)
	{
		float r = 0.0f;

		const __m128 a = _mm_set_ps(0.0f, 0.0f, _a.x, _a.y);
		const __m128 b = _mm_set_ps(0.0f, 0.0f, _b.x, _b.y);

		const __m128 mul = _mm_mul_ps(a, b);

		const __m128 shift = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(1, 1, 1, 1));

		const __m128 add = _mm_add_ps(mul, shift);

		r = _mm_cvtss_f32(add);

		return r;
	}

	static float Angle(Vec2 _a, Vec2 _b)
	{
		return acosf(Dot(_a, _b) / (_a.Length() * _b.Length()));
	}
};

template<>
class Vec2<int>
{
public:
	int x = 0;
	int y = 0;
public:
	Vec2()
	{
		x = 0;
		y = 0;
	}

	Vec2(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	Vec2& operator+=(const Vec2& _a)
	{
		const __m128i a = _mm_set_epi32(0, 0, x, y);
		const __m128i b = _mm_set_epi32(0, 0, _a.x, _a.y);

		const __m128i add = _mm_add_epi32(a, b);

		alignas(16) int value[4];
		_mm_store_si128(reinterpret_cast<__m128i*>(value), add);

		x = value[1];
		y = value[0];

		return *this;
	}
	Vec2 operator+(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp += _a;

		return tmp;
	}

	Vec2& operator-=(const Vec2& _a)
	{
		const __m128i a = _mm_set_epi32(0.0f, 0.0f, x, y);
		const __m128i b = _mm_set_epi32(0.0f, 0.0f, _a.x, _a.y);

		const __m128i sub = _mm_sub_epi32(a, b);

		alignas(16) int value[4];
		_mm_store_si128(reinterpret_cast<__m128i*>(value), sub);

		x = value[1];
		y = value[0];

		return *this;
	}
	Vec2 operator-(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp -= _a;

		return tmp;
	}

	Vec2& operator*=(const Vec2& _a)
	{
		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = _mm_set_ps(0.0f, 0.0f, _a.x, _a.y);

		const __m128 mul = _mm_mul_ps(a, b);

		alignas(16) float value[4];
		_mm_store_ps(value, mul);

		x = value[1];
		y = value[0];

		return *this;
	}
	Vec2 operator*(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec2& operator/=(const Vec2& _a)
	{
		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = _mm_set_ps(0.0f, 0.0f, _a.x, _a.y);

		const __m128 div = _mm_div_ps(a, b);

		alignas(16) float value[4];
		_mm_store_ps(value, div);

		x = value[1];
		y = value[0];

		return *this;
	}
	Vec2 operator/(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	Vec2& operator*=(const float& _a)
	{
		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = _mm_set_ps(0.0f, 0.0f, _a, _a);

		const __m128 mul = _mm_mul_ps(a, b);

		alignas(16) float value[4];
		_mm_store_ps(value, mul);

		x = value[1];
		y = value[0];

		return *this;
	}
	Vec2 operator*(const float& _a)
	{
		Vec2 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec2& operator/=(const float& _a)
	{
		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = _mm_set_ps(0.0f, 0.0f, _a, _a);

		const __m128 div = _mm_div_ps(a, b);

		alignas(16) float value[4];
		_mm_store_ps(value, div);

		x = value[1];
		y = value[0];

		return *this;
	}
	Vec2 operator/(const float& _a)
	{
		Vec2 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	bool operator==(const Vec2& _a) const
	{
		return x == _a.x && y == _a.y;
	}
	bool operator!=(const Vec2& _a) const
	{
		return !(*this == _a);
	}

	static Vec2 One()
	{
		return Vec2(1.0f, 1.0f);
	}

	static Vec2 Zero()
	{
		return Vec2(0.0f, 0.0f);
	}

	static Vec2 Up()
	{
		return Vec2(0.0f, 1.0f);
	}

	static Vec2 Down()
	{
		return Vec2(0.0f, -1.0f);
	}

	static Vec2 Right()
	{
		return Vec2(1.0f, 0.0f);
	}

	static Vec2 Left()
	{
		return Vec2(-1.0f, 0.0f);
	}

	Vec2<float> Normalize()
	{
		const float l = Length();

		Vec2<float> r = { 0.0f, 0.0f };

		const __m128 num = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 denom = _mm_set_ps(0.0f, 0.0f, l, l);

		const __m128 magn = _mm_div_ps(num, denom);

		alignas(16) float value[4] = { 0 };
		_mm_store_ps(value, magn);

		r = { value[1], value[0] };

		return r;
	}

	float Length()
	{
		float r = 0.0f;

		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = a;

		const __m128 pow = _mm_mul_ps(a, b);

		const __m128 shift = _mm_shuffle_ps(pow, pow, _MM_SHUFFLE(1, 1, 1, 1));

		const __m128 sum = _mm_add_ps(pow, shift);

		const __m128 root = _mm_sqrt_ps(sum);

		r = _mm_cvtss_f32(root);

		return r;
	}

	float LengthSqr()
	{
		float r = 0.0f;

		const __m128 a = _mm_set_ps(0.0f, 0.0f, x, y);
		const __m128 b = a;

		const __m128 pow = _mm_mul_ps(a, b);

		const __m128 shift = _mm_shuffle_ps(pow, pow, _MM_SHUFFLE(1, 1, 1, 1));

		const __m128 sum = _mm_add_ps(pow, shift);

		r = _mm_cvtss_f32(sum);

		return r;
	}

	static float Distance(Vec2 _a, Vec2 _b)
	{
		Vec2 l = _a - _b;

		return l.Length();
	}

	static float Dot(Vec2 _a, Vec2 _b)
	{
		float r = 0.0f;

		const __m128 a = _mm_set_ps(0.0f, 0.0f, _a.x, _a.y);
		const __m128 b = _mm_set_ps(0.0f, 0.0f, _b.x, _b.y);

		const __m128 mul = _mm_mul_ps(a, b);

		const __m128 shift = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(1, 1, 1, 1));

		const __m128 add = _mm_add_ps(mul, shift);

		r = _mm_cvtss_f32(add);

		return r;
	}

	static float Angle(Vec2 _a, Vec2 _b)
	{
		return acosf(Dot(_a, _b) / (_a.Length() * _b.Length()));
	}
};

#endif