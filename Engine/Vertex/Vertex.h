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

		Vertex();
		Vertex(glm::vec3 _pos);
		Vertex(glm::vec3 _pos, glm::vec3 _normal, glm::vec3 _color, glm::vec2 _texUV);
		~Vertex() = default;
	};
}

#endif