#include "Vector.h"

namespace Llyn
{
#pragma region Vector2
	Vector2u Vector2u::operator+(const Vector2u& _other) const
	{
		return { x + _other.x, y + _other.y };
	}

	Vector2u Vector2u::operator-(const Vector2u& _other) const
	{
		return { x - _other.x, y - _other.y };
	}

	Vector2u Vector2u::operator*(const Vector2u& _other) const
	{
		return { x * _other.x, y * _other.y };
	}

	Vector2u Vector2u::operator/(const Vector2u& _other) const
	{
		return { x / _other.x, y / _other.y };
	}

	Vector2i Vector2i::operator+(const Vector2i& _other) const
	{
		return { x + _other.x, y + _other.y };
	}

	Vector2i Vector2i::operator-(const Vector2i& _other) const
	{
		return { x - _other.x, y - _other.y };
	}

	Vector2i Vector2i::operator*(const Vector2i& _other) const
	{
		return { x * _other.x, y * _other.y };
	}

	Vector2i Vector2i::operator/(const Vector2i& _other) const
	{
		return { x / _other.x, y / _other.y };
	}

	Vector2f Vector2f::operator+(const Vector2f& _other) const
	{
		return { x + _other.x, y + _other.y };
	}

	Vector2f Vector2f::operator-(const Vector2f& _other) const
	{
		return { x - _other.x, y - _other.y };
	}

	Vector2f Vector2f::operator*(const Vector2f& _other) const
	{
		return { x * _other.x, y * _other.y };
	}

	Vector2f Vector2f::operator/(const Vector2f& _other) const
	{
		return { x / _other.x, y / _other.y };
	}
#pragma endregion

#pragma region Vector3
	Vector3u Vector3u::operator+(const Vector3u& _other) const
	{
		return { x + _other.x, y + _other.y, z + _other.z };
	}

	Vector3u Vector3u::operator-(const Vector3u& _other) const
	{
		return { x - _other.x, y - _other.y, z - _other.z };
	}

	Vector3u Vector3u::operator*(const Vector3u& _other) const
	{
		return { x * _other.x, y * _other.y, z * _other.z };
	}

	Vector3u Vector3u::operator/(const Vector3u& _other) const
	{
		return { x / _other.x, y / _other.y, z / _other.z };
	}

	Vector3i Vector3i::operator+(const Vector3i& _other) const
	{
		return { x + _other.x, y + _other.y, z + _other.z };
	}

	Vector3i Vector3i::operator-(const Vector3i& _other) const
	{
		return { x - _other.x, y - _other.y, z - _other.z };
	}

	Vector3i Vector3i::operator*(const Vector3i& _other) const
	{
		return { x * _other.x, y * _other.y, z * _other.z };
	}

	Vector3i Vector3i::operator/(const Vector3i& _other) const
	{
		return { x / _other.x, y / _other.y, z / _other.z };
	}

	Vector3f Vector3f::operator+(const Vector3f& _other) const
	{
		return { x + _other.x, y + _other.y, z + _other.z };
	}

	Vector3f Vector3f::operator-(const Vector3f& _other) const
	{
		return { x - _other.x, y - _other.y, x - _other.z };
	}

	Vector3f Vector3f::operator*(const Vector3f& _other) const
	{
		return { x * _other.x, y * _other.y, z * _other.z };
	}

	Vector3f Vector3f::operator/(const Vector3f& _other) const
	{
		return { x / _other.x, y / _other.y, z / _other.z };
	}
#pragma endregion

#pragma region Vector4
	Vector4u Vector4u::operator+(const Vector4u& _other) const
	{
		return { x + _other.x, y + _other.y, z + _other.z, w + _other.w };
	}

	Vector4u Vector4u::operator-(const Vector4u& _other) const
	{
		return { x - _other.x, y - _other.y, z - _other.z, w - _other.w };
	}

	Vector4u Vector4u::operator*(const Vector4u& _other) const
	{
		return { x * _other.x, y * _other.y, z * _other.z, w * _other.w };
	}

	Vector4u Vector4u::operator/(const Vector4u& _other) const
	{
		return { x / _other.x, y / _other.y, z / _other.z, w / _other.w };
	}

	Vector4i Vector4i::operator+(const Vector4i& _other) const
	{
		return { x + _other.x, y + _other.y, z + _other.z, w + _other.w };
	}

	Vector4i Vector4i::operator-(const Vector4i& _other) const
	{
		return { x - _other.x, y - _other.y, z - _other.z, w - _other.w };
	}

	Vector4i Vector4i::operator*(const Vector4i& _other) const
	{
		return { x * _other.x, y * _other.y, z * _other.z, w * _other.w };
	}

	Vector4i Vector4i::operator/(const Vector4i& _other) const
	{
		return { x / _other.x, y / _other.y, z / _other.z, w / _other.w };
	}

	Vector4f Vector4f::operator+(const Vector4f& _other) const
	{
		return { x + _other.x, y + _other.y, z + _other.z, w + _other.w };
	}

	Vector4f Vector4f::operator-(const Vector4f& _other) const
	{
		return { x - _other.x, y - _other.y, z - _other.z, w - _other.w };
	}

	Vector4f Vector4f::operator*(const Vector4f& _other) const
	{
		return { x * _other.x, y * _other.y, z * _other.z, w * _other.w };
	}

	Vector4f Vector4f::operator/(const Vector4f& _other) const
	{
		return { x / _other.x, y / _other.y, z / _other.z, w / _other.w };
	}
#pragma endregion

}
