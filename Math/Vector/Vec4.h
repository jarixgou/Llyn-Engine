#ifndef VEC_4__H
#define VEC_4__H

template <typename T>
class Vec4
{
public:
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;
public:
	Vec4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	Vec4(const T& _x, const T& _y, const T& _z, const T& _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	~Vec4() = default;

	Vec4& operator+=(const Vec4& _a)
	{
		x += _a.x;
		y += _a.y;
		z += _a.z;
		w += _a.w;

		return *this;
	}
	Vec4 operator+(const Vec4& _a)
	{
		Vec4 tmp = *this;
		tmp += _a;
		
		return _a;
	}

	Vec4& operator-=(const Vec4& _a)
	{
		x -= _a.x;
		y -= _a.y;
		z -= _a.z;
		w -= _a.w;
		
		return *this;
	}
	Vec4 operator-(const Vec4& _a)
	{
		Vec4 tmp = *this;
		tmp -= _a;

		return tmp;
	}

	Vec4& operator*=(const Vec4& _a)
	{
		x *= _a.x;
		y *= _a.y;
		z *= _a.z;
		w *= _a.w;

		return *this;
	}
	Vec4 operator*(const Vec4& _a)
	{
		Vec4 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec4& operator/=(const Vec4& _a)
	{
		x /= _a.x;
		y /= _a.y;
		z /= _a.z;
		w /= _a.w;

		return *this;
	}
	Vec4 operator/(const Vec4& _a)
	{
		Vec4 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	Vec4& operator*=(const float& _a)
	{
		x *= _a;
		y *= _a;
		z *= _a;
		w *= _a;

		return *this;
	}
	Vec4 operator*(const float& _a)
	{
		Vec4 tmp = *this;
		tmp *= _a;

		return tmp;
	}

	Vec4& operator/=(const float& _a)
	{
		x /= _a;
		y /= _a;
		z /= _a;
		w /= _a;

		return *this;
	}
	Vec4 operator/(const float& _a)
	{
		Vec4 tmp = *this;
		tmp /= _a;

		return tmp;
	}

	static Vec4 One()
	{
		return { 1, 1, 1, 1 };
	}

	static Vec4 Zero()
	{
		return { 0, 0, 0, 0 };
	}

	float Length()
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float LengthSqr()
	{
		return x * x + y * y + z * z + w * w;
	}

	Vec4<float> Normalize()
	{
		const float l = Length();

		return { x / l, y / l, z / l, w / l };
	}

	static float Distance(const Vec4& _a, const Vec4& _b)
	{
		const Vec4 d = _b - _a;

		return d.Length();
	}

	static float Dot(const Vec4& _a, const Vec4& _b)
	{
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
	}

	static Vec4<float> Lerp(const Vec4& _a, const Vec4& _b, const float& _t)
	{
		return _a + (_b - _a) * _t;
	}

	static Vec4<float> Project(const Vec4& _a, const Vec4& _onNormal)
	{
		return (Dot(_a, _onNormal) / _onNormal.LengthSqr()) * _onNormal;
	}
};

#endif