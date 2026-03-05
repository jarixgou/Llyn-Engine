#include "Material.h"

#include "../Texture/Texture.h"
#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/Shader/Shader.h"

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

	void Material::Bind(Shader& _shader) const
	{
		if (baseMap != nullptr)
		{
			baseMap->Bind();
			baseMap->TexUnit(_shader, "material.baseMap", 0);
		}
		_shader.SetUniform("material.baseColor", baseColor);

		if (specularMap != nullptr)
		{
			specularMap->Bind();
			specularMap->TexUnit(_shader, "material.specularMap", 1);
		}
		_shader.SetUniform("material.specularColor", specularColor);

		if (normalMap != nullptr)
		{
			normalMap->Bind();
			normalMap->TexUnit(_shader, "material.normalMap", 2);
		}

		_shader.SetUniform("material.shininess", shininess);
	}
}
