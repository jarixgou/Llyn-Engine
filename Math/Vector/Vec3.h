#ifndef VEC_3__H
#define VEC_3__H
#include <cmath>

#include "../Math.h"

template<typename T>
class Vec3
{
public:
	T x = 0;
	T y = 0;
	T z = 0;
public:
	Vec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vec3(const T& _x, const T& _y, const T& _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	~Vec3() = default;

	Vec3& operator+=(const Vec3& _a)
	{
		x += _a.x;
		y += _a.y;
		z += _a.z;

		return *this;
	}
	Vec3 operator+(const Vec3& _a)
	{
		Vec3 tmp = *this;
		tmp += _a;

		return tmp;
	}

	Vec3& operator-=(const Vec3& _a)
	{
		x -= _a.x;
		y -= _a.y;
		z -= _a.z;

		return *this;
	}
	Vec3 operator-(const Vec3& _a)
	{
		Vec3 tmp = *this;
		tmp -= _a;

		return tmp;
	}

	Vec3& operator*=(const Vec3& _a)
	{
		x *= _a.x;
		y *= _a.y;
		z *= _a.z;

		return *this;
	}
	Vec3 operator*(const Vec3& _a)
	{
		Vec3 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec3& operator/=(const Vec3& _a)
	{
		x /= _a.x;
		y /= _a.y;
		z /= _a.z;

		return *this;
	}
	Vec3 operator/(const Vec3& _a)
	{
		Vec3 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	Vec3& operator*=(const float& _a)
	{
		x *= _a;
		y *= _a;
		z *= _a;

		return *this;
	}
	Vec3 operator*(const float& _a)
	{
		Vec3 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec3& operator/=(const float& _a)
	{
		x /= _a;
		y /= _a;
		z /= _a;

		return *this;
	}
	Vec3 operator/(const float& _a)
	{
		Vec3 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	bool operator==(const Vec3& _a) const
	{
		return x == _a.x && y == _a.y && z == _a.z;
	}
	bool operator!=(const Vec3& _a) const
	{
		return !(*this == _a);
	}

	static Vec3 One()
	{
		return { 1, 1, 1 };
	}

	static Vec3 Zero()
	{
		return { 0, 0, 0 };
	}

	static Vec3 Up()
	{
		return { 0, 1, 0 };
	}
	static Vec3 Down()
	{
		return { 0, -1, 0 };
	}

	static Vec3 Right()
	{
		return { 1, 0, 0 };
	}
	static Vec3 Left()
	{
		return { -1, 0, 0 };
	}

	static Vec3 Forward()
	{
		return { 0, 0, 1 };
	}
	static Vec3 Back()
	{
		return { 0, 0, -1 };
	}

	float Length()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSqr()
	{
		return x * x + y * y + z * z;
	}

	Vec3<float> Normalize()
	{
		const float l = Length();

		return { x / l, y / l, z / l };
	}

	static float Distance(const Vec3& _a, const Vec3 _b)
	{
		const Vec3 d = _b - _a;

		return d.Length();
	}

	static float Dot(const Vec3& _a, const Vec3& _b)
	{
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	}

	static float Angle(const Vec3& _a, const Vec3& _b)
	{
		return acosf(Dot(_a, _b) / (_a.Length() * _b.Length()));
	}

	static Vec3 Cross(const Vec3& _a, const Vec3& _b)
	{
		return {
			_a.y * _b.z - _a.z * _b.y,
			_a.z * _b.x - _a.x * _b.z,
			_a.x * _b.y - _a.y * _b.x
		};
	}

	static Vec3 Lerp(const Vec3& _a, const Vec3& _b, const float& _t)
	{
		return _a + (_b - _a) * _t;
	}

	static Vec3 SLerp(const Vec3& _a, const Vec3& _b, const float& _t)
	{
		const float angle = Angle(_a, _b);

		const float sinAngle = sinf(angle);

		return (sinf((1 - _t) * angle) / sinAngle) * _a + (sinf(angle * _t) / sinAngle) * _b;
	}

	static Vec3<float> Project(const Vec3& _a, const Vec3& _onNormal)
	{
		return (Dot(_a, _onNormal) / _onNormal.LengthSqr()) * _onNormal;
	}

	static Vec3<float> ProjectOnPlane(const Vec3& _a, const Vec3& _planeNormal)
	{
		return _a - Project(_a, _planeNormal);
	}

	static Vec3 Reflect(const Vec3& _dir, const Vec3& _normal)
	{
		return _dir - 2 * Dot(_dir, _normal) * _normal;
	}
};

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
typedef Vec3<unsigned int> Vec3u;

#endif