#ifndef MATERIAL__H
#define MATERIAL__H
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace Llyn
{
	class Texture;
	class Shader;

	struct Transform;
	class Camera;

	struct Material
	{
		Shader* shader;

		Texture* baseMap;
		Texture* specularMap;
		Texture* normalMap;

		glm::vec3 baseColor;
		glm::vec3 specularColor;

		float shininess;

		Material();
		~Material();

		void Bind(Camera* _camera, glm::mat4 _model) const;
	};
}

#endif