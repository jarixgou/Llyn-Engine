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
	Vec4 operator-(const Vec4 _a)
	{
		Vec4 tmp = *this;
		tmp -= _a;

		return tmp;
	}
};

#endif