#ifndef DIRECT_LIGHT_DATA__H
#define DIRECT_LIGHT_DATA__H
#include <glm/vec3.hpp>

namespace Llyn
{
	struct alignas(16) DirectLightData
	{
		glm::vec3 dir = { 0.f, -1.f, 0.f };
		float padding;

		glm::vec3 ambient = { 1.f, 1.f, 1.f };
		float padding1;

		glm::vec3 diffuse = { 1.f, 1.f, 1.f };
		float padding2;

		glm::vec3 specular = { 1.f, 1.f, 1.f };
		float padding3;
	};
}

#endif