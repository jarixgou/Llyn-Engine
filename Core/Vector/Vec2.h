#ifndef VEC2__H
#define VEC2__H

template<typename T>
struct Vec2
{
	T x = 0;
	T y = 0;

	Vec2& operator+=(const Vec2& _other);
	Vec2 operator+(const Vec2& _other);

	Vec2& operator-=(const Vec2& _other);
	Vec2 operator-(const Vec2& _other);

	Vec2& operator*=(const Vec2& _other);
	Vec2 operator*(const Vec2& _other);

	Vec2& operator/=(const Vec2& _other);
	Vec2 operator/(const Vec2& _other);

	bool operator==(const Vec2& _other) const;
	bool operator!=(const Vec2& _other) const;
};

template <typename T>
Vec2<T>& Vec2<T>::operator+=(const Vec2& _other)
{
	this->x += _other.x;
	this->y += _other.y;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator+(const Vec2& _other)
{
	Vec2 tmp = *this;
	tmp += _other;
	return tmp;
}

template <typename T>
Vec2<T>& Vec2<T>::operator-=(const Vec2& _other)
{
	this->x -= _other.x;
	this->y -= _other.y;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2& _other)
{
	Vec2 tmp = *this;
	tmp -= _other;
	return tmp;
}

template <typename T>
Vec2<T>& Vec2<T>::operator*=(const Vec2& _other)
{
	this->x *= _other.x;
	this->y *= _other.y;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator*(const Vec2& _other)
{
	Vec2 tmp = *this;
	tmp *= _other;
	return tmp;
}

template <typename T>
Vec2<T>& Vec2<T>::operator/=(const Vec2& _other)
{
	this->x /= _other.x;
	this->y /= _other.y;

	return *this;
}

template <typename T>
Vec2<T> Vec2<T>::operator/(const Vec2& _other)
{
	Vec2 tmp = *this;
	tmp /= _other;
	return tmp;
}

template <typename T>
bool Vec2<T>::operator==(const Vec2& _other) const
{
	return this->x == _other.x && this->y == _other.y;
}

template <typename T>
bool Vec2<T>::operator!=(const Vec2& _other) const
{
	return !(*this == _other);
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned int>;

#endif
