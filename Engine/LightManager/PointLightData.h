#ifndef POINT_LIGHT_DATA__H
#define POINT_LIGHT_DATA__H
#include <glm/vec3.hpp>

namespace Llyn
{
	struct alignas(16) PointLightData
	{
		glm::vec3 position;
		float constant;

		glm::vec3 ambient;
		float linear;

		glm::vec3 diffuse;
		float quadratic;

		glm::vec3 specular;
		float padding;
	};
}

#endif