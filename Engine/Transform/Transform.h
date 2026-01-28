#ifndef TRANSFORM__H
#define TRANSFORM__H

#include <glm/vec3.hpp>

#include "../Macro.h"

namespace ENGINE_NAME
{
	struct Transform
	{
		Transform();

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};
}

#endif