#include "Transform.h"

#include "../Math/Math.h"
#include "../Math/Matrix/Mat4.h"

Mat4 Transform::GetMatrix() const
{
	Mat4 model = IdentityMat4();

	model = Translate(model, pos);
	model *= rot.GetMatrix();
	model = Scale(model, scale);

	return model;
}

Transform Transform::operator+(const Transform& _other)
{
	Transform transform;

	transform.pos = pos + _other.pos;
	transform.rot = rot * _other.rot;
	transform.scale = scale + _other.scale;

	return transform;
}

Transform& Transform::operator+=(const Transform& _other)
{
	Transform transform = *this + _other;
	return transform;
}

Transform Transform::operator*(const Transform& _other)
{
	Transform transform;

	transform.pos = pos * _other.pos;
	transform.rot = rot * _other.rot;
	transform.scale = scale * _other.scale;
	
	return transform;
}

Transform Transform::operator*=(const Transform& _other)
{
	Transform transform = *this * _other;
	return transform;
}
