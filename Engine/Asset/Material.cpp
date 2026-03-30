#include "Material.h"

#include <fstream>

#include "../Camera/Camera.h"
#include "../Texture/Texture.h"
#include "../Memory/MemoryHelper.h"
#include "../Component/Transform.h"
#include "../Render/OpenGL/Shader/Shader.h"
#include <glm/mat4x4.hpp>

#include <json/json.hpp>

#include "AssetManager.h"

namespace Llyn
{
	Material::Material()
	{
		shader = nullptr;

		baseMap = nullptr;
		specularMap = nullptr;
		normalMap = nullptr;

		baseColor = glm::vec3(1.f, 1.f, 1.f);
		specularColor = glm::vec3(1.f, 1.f, 1.f);

		shininess = 32.f;
	}

	Material::~Material()
	{
		shader = nullptr;

		baseMap = nullptr;
		specularMap = nullptr;
		normalMap = nullptr;
	}

	bool Material::Load(const char* _path)
	{
		shader = nullptr;

		baseMap = nullptr;
		specularMap = nullptr;
		normalMap = nullptr;

		baseColor = glm::vec3(1.f, 1.f, 1.f);
		specularColor = glm::vec3(1.f, 1.f, 1.f);

		shininess = 32.f;

		std::ifstream file(_path);

		if (!file.is_open())
		{
			return false;
		}

		nlohmann::json data = nlohmann::json::parse(file);

		file.close();

		if (data.contains("Shader"))
		{
			auto& shad = data["Shader"];

			std::string vertexPath;
			std::string fragmentPath;
			if (shad.contains("Vertex") && shad["Vertex"].is_string())
			{
				vertexPath = shad["Vertex"].get<std::string>();
			}
			if (shad.contains("Fragment") && shad["Fragment"].is_string())
			{
				fragmentPath = shad["Fragment"].get<std::string>();
			}

			ALLOCATE_MEMORY(shader, vertexPath.c_str(), fragmentPath.c_str());
		}

		if (data.contains("Textures") && data["Textures"].is_object())
		{
			auto& texture = data["Textures"];

			if (texture.contains("baseMap") && texture["baseMap"].is_string())
			{
				std::string path = texture["baseMap"].get<std::string>();
				if (!path.empty())
				{
					baseMap = AssetManager::Get()->GetAsset<Texture>(path.c_str());
					baseMap->SetSlot(0);
				}
			}
			if (texture.contains("specularMap") && texture["specularMap"].is_string())
			{
				std::string path = texture["specularMap"].get<std::string>();
				if (!path.empty())
				{
					specularMap = AssetManager::Get()->GetAsset<Texture>(path.c_str());
					specularMap->SetSlot(1);
				}
			}
			if (texture.contains("normalMap") && texture["normalMap"].is_string())
			{
				std::string path = texture["normalMap"].get<std::string>();
				if (!path.empty())
				{
					normalMap = AssetManager::Get()->GetAsset<Texture>(path.c_str());
					normalMap->SetSlot(2);
				}
			}
		}

		if (data.contains("baseColor") && data["baseColor"].is_array() && data["baseColor"].size() >= 3)
		{
			
			baseColor.r = data["baseColor"][0].get<float>();
			baseColor.g = data["baseColor"][1].get<float>();
			baseColor.b = data["baseColor"][2].get<float>();
		}

		if (data.contains("specularColor") && data["specularColor"].is_array() && data["specularColor"].size() >= 3)
		{

			baseColor.r = data["specularColor"][0].get<float>();
			baseColor.g = data["specularColor"][1].get<float>();
			baseColor.b = data["specularColor"][2].get<float>();
		}

		if (data.contains("shininess") && data["shininess"].is_number())
		{
			shininess = data["shininess"].get<float>();
		}
		return true;
	}

	bool Material::Save()
	{
		nlohmann::json data;
		nlohmann::json textures;

		if (baseMap != nullptr)
		{
			textures["baseMap"] = baseMap->GetPath();
		}
		if (specularMap != nullptr)
		{
			textures["specularMap"] = specularMap->GetPath();
		}
		if (normalMap != nullptr)
		{
			textures["normalMap"] = normalMap->GetPath();
		}

		data["Textures"] = textures;

		data["baseColor"] = { baseColor.r, baseColor.g, baseColor.b };
		data["specularColor"] = { specularColor.r, specularColor.g, specularColor.b };

		data["shininess"] = shininess;

		std::ofstream file(GetPath());
		if (!file.is_open())
		{
			return false;
		}

		file << data.dump(4);
		
		file.close();

		return true;
	}

	void Material::Bind(Camera* _camera, glm::mat4 _model) const
	{
		if (_camera != nullptr && shader != nullptr)
		{
			shader->Activate();

			_camera->Matrix(shader);
			shader->SetUniform("camPos", _camera->GetPosition());
			shader->SetUniform("model", glm::value_ptr(_model), 1);

			shader->SetUniform("light.type", 1);
			shader->SetUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
			shader->SetUniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
			shader->SetUniform("light.specular", glm::vec3(1.f, 1.f, 1.f));
			shader->SetUniform("light.position", glm::vec3(0.f, 5.f, 0.f));
			shader->SetUniform("light.direction", glm::vec3(0.f, -1.f, 0.f));
			shader->SetUniform("light.constant", 1.f);
			shader->SetUniform("light.linear", 0.09f);
			shader->SetUniform("light.quadratic", 0.032f);

			if (baseMap != nullptr)
			{
				baseMap->Bind();
				baseMap->TexUnit(*shader, "material.baseMap");
			}
			shader->SetUniform("material.baseColor", baseColor);

			if (specularMap != nullptr)
			{
				specularMap->Bind();
				specularMap->TexUnit(*shader, "material.specularMap");
			}
			shader->SetUniform("material.specularColor", specularColor);

			if (normalMap != nullptr)
			{
				normalMap->Bind();
				normalMap->TexUnit(*shader, "material.normalMap");
			}

			shader->SetUniform("material.shininess", shininess);
		}
	}

	Material& Material::operator=(const Material& _other)
	{
		if (this == &_other)
		{
			return *this;
		}

		shader = _other.shader;

		baseMap = _other.baseMap;
		specularMap = _other.specularMap;
		normalMap = _other.normalMap;

		baseColor = _other.baseColor;
		specularColor = _other.specularColor;

		shininess = _other.shininess;

		return *this;
	}
}
