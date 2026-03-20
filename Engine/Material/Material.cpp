#include "Material.h"

#include "../Camera/Camera.h"
#include "../Texture/Texture.h"
#include "../Memory/MemoryHelper.h"
#include "../Component/Transform.h"
#include "../Render/OpenGL/Shader/Shader.h"
#include <glm/mat4x4.hpp>

namespace Llyn
{
	Material::Material()
	{
		baseMap = nullptr;
		specularMap = nullptr;
		normalMap = nullptr;

		baseColor = glm::vec3(1.f, 1.f, 1.f);
		specularColor = glm::vec3(1.f, 1.f, 1.f);

		shininess = 32.f;
	}

	Material::~Material()
	{
		baseMap = nullptr;
		specularMap = nullptr;
		normalMap = nullptr;
	}

	void Material::Bind(Camera* _camera, glm::mat4 _model) const
	{
		if (_camera != nullptr && shader != nullptr)
		{
			shader->Activate();

			_camera->Matrix(shader);
			shader->SetUniform("uCamPos", _camera->GetPosition());
			shader->SetUniform("model", glm::value_ptr(_model), 1);

			if (baseMap != nullptr)
			{
				baseMap->Bind();
				baseMap->TexUnit(*shader, "material.baseMap", 0);
			}
			shader->SetUniform("material.baseColor", baseColor);

			if (specularMap != nullptr)
			{
				specularMap->Bind();
				specularMap->TexUnit(*shader, "material.specularMap", 1);
			}
			shader->SetUniform("material.specularColor", specularColor);

			if (normalMap != nullptr)
			{
				normalMap->Bind();
				normalMap->TexUnit(*shader, "material.normalMap", 2);
			}

			shader->SetUniform("material.shininess", shininess);
		}
	}
}
