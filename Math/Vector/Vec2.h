#ifndef VEC_2__H
#define VEC_2__H
#include <cmath>

template<typename T>
class Vec2
{
public:
	T x = 0;
	T y = 0;
public:
	Vec2()
	{
		x = 0;
		y = 0;
	}
	Vec2(const T& _x, const T& _y)
	{
		x = _x;
		y = _y;
	}

	~Vec2() = default;

	Vec2& operator+=(const Vec2& _a)
	{
		x += _a.x;
		y += _a.y;

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
		x -= _a.x;
		y -= _a.y;

		return *this;
	}
	Vec2 operator-(const Vec2& _a)
	{
		Vec2 tmp = *this;
		tmp -= _a;

		return _a;
	}

	Vec2& operator*=(const Vec2& _a)
	{
		x *= _a.x;
		y *= _a.y;

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
		x /= _a.x;
		y /= _a.y;

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
		x *= _a;
		y *= _a;

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
		Vec2 tmp = *this;
		tmp /= _a;

		return tmp;
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
		return { 1, 1 };
	}

	static Vec2 Zero()
	{
		return { 0, 0 };
	}

	static Vec2 Up()
	{
		return { 0, 1 };
	}
	static Vec2 Down()
	{
		return { 0, -1 };
	}

	static Vec2 Right()
	{
		return { 1, 0 };
	}
	static Vec2 Left()
	{
		return { -1, 0 };
	}

	float Length()
	{
		return sqrtf(x * x + y * y);
	}

	float LengthSqr()
	{
		return x * x + y * y;
	}

	Vec2<float> Normalize()
	{
		const float	l = Length();

		return { x / l, y / l };
	}

	static float Distance(const Vec2& _a, const Vec2& _b)
	{
		const Vec2 d = _a - _b;

		return d.Length();
	}

	static float Dot(const Vec2& _a, const Vec2& _b)
	{
		return _a.x * _b.x + _a.y * _b.y;
	}

	static float Angle(const Vec2& _a, const Vec2& _b)
	{
		return acosf(Dot(_a, _b) / (_a.Length() * _b.Length()));
	}
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec2<unsigned int> Vec2u;

#endif