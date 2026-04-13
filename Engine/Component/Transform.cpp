#include "Transform.h"

#include <glm/gtc/quaternion.hpp>

namespace Llyn
{
	Transform::Transform()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		rotation = glm::quat(0.0f, 0.f, 0.f, 0.f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	Transform::~Transform() = default;

	glm::mat4 Transform::GetMatrix()
	{
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);
		return model;
	}

	void Transform::Draw(Camera* _camera, Transform* _transform)
	{

	}

	Transform& Transform::operator+=(Transform& _other)
	{
	}

	Transform& Transform::operator+(Transform& _other)
	{
	}
}
