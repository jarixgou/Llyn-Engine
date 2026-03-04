#include "Transform.h"

namespace Llyn
{
	Transform::Transform()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		rotation = glm::quat(1.0f, 0.f, 0.f, 0.f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}
}
