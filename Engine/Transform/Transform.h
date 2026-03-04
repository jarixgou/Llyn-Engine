#ifndef TRANSFORM__H
#define TRANSFORM__H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/detail/type_quat.hpp>

namespace Llyn
{
	struct Transform
	{
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;

		Transform();
	};
}

#endif