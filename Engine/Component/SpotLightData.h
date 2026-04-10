#ifndef SPOT_LIGHT_DATA__H
#define SPOT_LIGHT_DATA__H
#include <glm/vec3.hpp>

namespace Llyn
{
	struct SpotLightData
	{
		glm::vec3 position;
		float cutOff;

		glm::vec3 direction;
		float outerCutOff;

		glm::vec3 ambient;
		float padding;

		glm::vec3 diffuse;
		float padding1;

		glm::vec3 specular;
		float padding;
	};
}

#endif