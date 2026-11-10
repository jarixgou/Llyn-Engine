#ifndef VEC_3__H
#define VEC_3__H

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
};

#endif