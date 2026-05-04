#ifndef VEC4__H
#define VEC4__H

template<typename T>
struct Vec4
{
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	Vec4& operator+=(const Vec4& _other);
	Vec4 operator+(const Vec4& _other) const;

	Vec4& operator-=(const Vec4& _other);
	Vec4 operator-(const Vec4& _other) const;

	Vec4& operator*=(const Vec4& _other);
	Vec4 operator*(const Vec4& _other) const;

	Vec4& operator/=(const Vec4& _other);
	Vec4 operator/(const Vec4& _other) const;

	bool operator==(const Vec4& _other) const;
	bool operator!=(const Vec4& _other) const;
};

template <typename T>
Vec4<T>& Vec4<T>::operator+=(const Vec4& _other)
{
	this->x += _other.x;
	this->y += _other.y;
	this->z += _other.z;
	this->w += _other.w;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator+(const Vec4& _other) const
{
	Vec4 tmp = *this;
	tmp += _other;
	return tmp;
}

template <typename T>
Vec4<T>& Vec4<T>::operator-=(const Vec4& _other)
{
	this->x -= _other.x;
	this->y -= _other.y;
	this->z -= _other.z;
	this->w -= _other.w;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator-(const Vec4& _other) const
{
	Vec4 tmp = *this;
	tmp -= _other;
	return tmp;
}

template <typename T>
Vec4<T>& Vec4<T>::operator*=(const Vec4& _other)
{
	this->x *= _other.x;
	this->y *= _other.y;
	this->z *= _other.z;
	this->w *= _other.w;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator*(const Vec4& _other) const
{
	Vec4 tmp = *this;
	tmp *= _other;
	return tmp;
}

template <typename T>
Vec4<T>& Vec4<T>::operator/=(const Vec4& _other)
{
	this->x /= _other.x;
	this->y /= _other.y;
	this->z /= _other.z;
	this->w /= _other.w;

	return *this;
}

template <typename T>
Vec4<T> Vec4<T>::operator/(const Vec4& _other) const
{
	Vec4 tmp = *this;
	tmp /= _other;
	return tmp;
}

template <typename T>
bool Vec4<T>::operator==(const Vec4& _other) const
{
	return this->x == _other.x && this->y == _other.y && this->z == _other.z &&
		this->w == _other.w;
}

template <typename T>
bool Vec4<T>::operator!=(const Vec4& _other) const
{
	return !(*this == _other);
}

using Vec4f = Vec4<float>;
using Vec4i = Vec4<int>;
using Vec4u = Vec4<unsigned int>;

#endif