#ifndef MATERIAL__H
#define MATERIAL__H
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "Asset.h"

namespace Llyn
{
	class Texture;
	class Shader;

	struct Transform;
	class Camera;

	struct Material : public Asset
	{
		Shader* shader;

		Texture* baseMap;
		Texture* specularMap;
		Texture* normalMap;

		glm::vec3 baseColor;
		glm::vec3 specularColor;

		float shininess;

		Material();
		~Material() override;

		bool Load(const char* _path) override;
		bool Save() override;

		void Bind(Camera* _camera, glm::mat4 _model) const;

		Material& operator=(const Material& _other);
	};
}

#endif