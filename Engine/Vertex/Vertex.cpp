#include "Vertex.h"

namespace Llyn
{
	Vertex::Vertex()
	{
		position = glm::vec3(0.f, 0.f, 0.f);
		normal = glm::vec3(0.f, 0.f, 0.f);
		color = glm::vec3(1.f, 1.f, 1.f);
		texUV = {0.f, 0.f};
	}

	Vertex::Vertex(glm::vec3 _pos) : position(_pos)
	{
		normal = glm::vec3(0.f, 0.f, 0.f);
		color = glm::vec3(1.f, 1.f, 1.f);
		texUV = { 0.f, 0.f };
	}

	Vertex::Vertex(glm::vec3 _pos, glm::vec3 _normal, glm::vec3 _color, glm::vec2 _texUV) :
	position(_pos), normal(_normal), color(_color), texUV(_texUV)
	{
	}
}
