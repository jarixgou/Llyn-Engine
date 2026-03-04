#ifndef VECTOR__H
#define VECTOR__H

#include <glad/glad.h>

namespace Llyn
{
#pragma region Vector2 Definitions
	struct Vector2u
	{
		GLuint x;
		GLuint y;

		Vector2u operator+(const Vector2u& _other) const;
		Vector2u operator-(const Vector2u& _other) const;
		Vector2u operator*(const Vector2u& _other) const;
		Vector2u operator/(const Vector2u& _other) const;
	};

	struct Vector2i
	{
		GLint x;
		GLint y;

		Vector2i operator+(const Vector2i& _other) const;
		Vector2i operator-(const Vector2i& _other) const;
		Vector2i operator*(const Vector2i& _other) const;
		Vector2i operator/(const Vector2i& _other) const;
	};

	struct Vector2f
	{
		GLfloat x;
		GLfloat y;

		Vector2f operator+(const Vector2f& _other) const;
		Vector2f operator-(const Vector2f& _other) const;
		Vector2f operator*(const Vector2f& _other) const;
		Vector2f operator/(const Vector2f& _other) const;
	};
#pragma endregion

#pragma region Vector3 Definitions
	struct Vector3u
	{
		GLuint x;
		GLuint y;
		GLuint z;

		Vector3u operator+(const Vector3u& _other) const;
		Vector3u operator-(const Vector3u& _other) const;
		Vector3u operator*(const Vector3u& _other) const;
		Vector3u operator/(const Vector3u& _other) const;
	};

	struct Vector3i
	{
		GLint x;
		GLint y;
		GLint z;

		Vector3i operator+(const Vector3i& _other) const;
		Vector3i operator-(const Vector3i& _other) const;
		Vector3i operator*(const Vector3i& _other) const;
		Vector3i operator/(const Vector3i& _other) const;
	};

	struct Vector3f
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;

		Vector3f operator+(const Vector3f& _other) const;
		Vector3f operator-(const Vector3f& _other) const;
		Vector3f operator*(const Vector3f& _other) const;
		Vector3f operator/(const Vector3f& _other) const;
	};
#pragma endregion

#pragma region Vector4 Definitions
	struct Vector4u
	{
		GLuint x;
		GLuint y;
		GLuint z;
		GLuint w;

		Vector4u operator+(const Vector4u& _other) const;
		Vector4u operator-(const Vector4u& _other) const;
		Vector4u operator*(const Vector4u& _other) const;
		Vector4u operator/(const Vector4u& _other) const;
	};
	struct Vector4i
	{
		GLint x;
		GLint y;
		GLint z;
		GLint w;

		Vector4i operator+(const Vector4i& _other) const;
		Vector4i operator-(const Vector4i& _other) const;
		Vector4i operator*(const Vector4i& _other) const;
		Vector4i operator/(const Vector4i& _other) const;
	};
	struct Vector4f
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat w;

		Vector4f operator+(const Vector4f& _other) const;
		Vector4f operator-(const Vector4f& _other) const;
		Vector4f operator*(const Vector4f& _other) const;
		Vector4f operator/(const Vector4f& _other) const;
	};
#pragma endregion
}

#endif