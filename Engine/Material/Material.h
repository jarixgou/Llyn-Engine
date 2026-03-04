#ifndef MATERIAL__H
#define MATERIAL__H
#include <glm/vec3.hpp>

namespace Llyn
{
	class Texture;
	class Shader;

	struct Material
	{
		Texture* baseMap;
		Texture* specularMap;
		Texture* normalMap;

		glm::vec3 baseColor;
		glm::vec3 specularColor;

		float shininess;

		Material();
		~Material();

		void Bind(Shader& _shader) const;
	};
}

#endif