#ifndef VEC3__H
#define VEC3__H

template<typename T>
struct Vec3
{
	T x = 0;
	T y = 0;
	T z = 0;

	Vec3& operator+=(const Vec3& _other);
	Vec3 operator+(const Vec3& _other);

	Vec3& operator-=(const Vec3& _other);
	Vec3 operator-(const Vec3& _other);

	Vec3& operator*=(const Vec3& _other);
	Vec3 operator*(const Vec3& _other);

	Vec3& operator/=(const Vec3& _other);
	Vec3 operator/(const Vec3& _other);

	bool operator==(const Vec3& _other);
	bool operator!=(const Vec3& _other);
};

template <typename T>
Vec3<T>& Vec3<T>::operator+=(const Vec3& _other)
{
	this->x += _other.x;
	this->y += _other.y;
	this->z += _other.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator+(const Vec3& _other)
{
	Vec3 tmp = *this;
	tmp += _other;
	return tmp;
}

template <typename T>
Vec3<T>& Vec3<T>::operator-=(const Vec3& _other)
{
	this->x -= _other.x;
	this->y -= _other.y;
	this->z -= _other.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator-(const Vec3& _other)
{
	Vec3 tmp = *this;
	tmp -= _other;
	return tmp;
}

template <typename T>
Vec3<T>& Vec3<T>::operator*=(const Vec3& _other)
{
	this->x *= _other.x;
	this->y *= _other.y;
	this->z *= _other.z;

	return *this;
}

template <typename T>
Vec3<T> Vec3<T>::operator*(const Vec3& _other)
{
	Vec3 tmp = *this;
	tmp *= _other;
	return tmp;
}

template <typename T>
Vec3<T>& Vec3<T>::operator/=(const Vec3& _other)
{
	this->x /= _other.x;
	this->y /= _other.y;
	this->z /= _other.z;
}

template <typename T>
Vec3<T> Vec3<T>::operator/(const Vec3& _other)
{
	Vec3 tmp = *this;
	tmp /= _other;
	return tmp;
}

template <typename T>
bool Vec3<T>::operator==(const Vec3& _other)
{
	return this->x == _other.x && this->y == _other.y && this->z == _other.z;
}

template <typename T>
bool Vec3<T>::operator!=(const Vec3& _other)
{
	return !(*this == _other);
}

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<unsigned int>;

#endif