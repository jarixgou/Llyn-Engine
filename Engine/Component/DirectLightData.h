#ifndef DIRECT_LIGHT_DATA__H
#define DIRECT_LIGHT_DATA__H
#include <glm/vec3.hpp>

namespace Llyn
{
	struct DirectLightData
	{
		glm::vec3 dir;
		float padding;

		glm::vec3 ambient;
		float padding1;

		glm::vec3 diffuse;
		float padding2;

		glm::vec3 specular;
		float padding3;
	};
}

#endif