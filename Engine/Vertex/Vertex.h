#ifndef VERTEX__H
#define VERTEX__H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Llyn
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 texUV;
	};
}

#endif