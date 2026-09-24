#include "Material.h"

#include <fstream>
#include <JSON/simdjson.h>
#include <JSON/json.hpp>

#include "RessourceManager.h"
#include "Texture.h"
#include "../Utils/Utils.h"

Material::~Material()
{
}

bool Material::Load(const std::string& _fileName)
{
	if (!Utils::CheckFileExtension(_fileName, "mat"))
	{
		LOGGER_ERROR("Not the right extensions to load mat");
		return false;
	}

	simdjson::padded_string json;
	if (simdjson::padded_string::load(_fileName).get(json))
	{
		LOGGER_ERROR("Failed to load json file");
		return false; // Failed to load json file
	}

	simdjson::ondemand::parser parser;
	simdjson::ondemand::document data;

	if (parser.iterate(json).get(data))
	{
		LOGGER_ERROR("Failed to parse json");
		return false; // Failed to parse json
	}

	simdjson::ondemand::object root = data.get_object();
	for (auto field : root)
	{
		std::string_view key = field.unescaped_key();
		if (key == "BaseMap")
		{
			simdjson::ondemand::object baseMap = field.value().get_object();
			for (auto baseField : baseMap)
			{
				std::string_view baseKey = baseField.unescaped_key();
				if (baseKey == "Texture" && baseField.value().is_string())
				{
					std::string_view texturePath = baseField.value().get_string();
					m_baseID = RessourceManager::Get().GetRessourceID<Texture>(std::string(texturePath));
				}
				else if (baseKey == "Color")
				{
					float color[3] = { 0.0f };
					int i = 0;
					for (auto value : baseField.value().get_array())
					{
						if (value.is_scalar())
						{
							if (i < 3)
							{
								color[i] = static_cast<float>(value.get_double());

								i += 1;
							}
						}
					}

					m_baseColor = { color[0], color[1], color[2] };
				}
			}
		}
		else if (key == "SpecularMap")
		{
			simdjson::ondemand::object specularMap = field.value().get_object();
			for (auto specularField : specularMap)
			{
				std::string_view specularKey = specularField.unescaped_key();
				if (specularKey == "Texture" && specularField.value().is_string())
				{
					std::string_view texturePath = specularField.value().get_string();
					m_specularID = RessourceManager::Get().GetRessourceID<Texture>(std::string(texturePath));
				}
				else if (specularKey == "Color")
				{
					float color[3] = { 0.0f };
					int i = 0;
					for (auto value : specularField.value().get_array())
					{
						if (value.is_scalar())
						{
							if (i < 3)
							{
								color[i] = static_cast<float>(value.get_double());

								i += 1;
							}
						}
					}

					m_specularColor = { color[0], color[1], color[2] };
				}
				else if (specularKey == "Smoothness" && specularField.value().is_scalar())
				{
					m_smoothness = static_cast<float>(specularField.value().get_double());
				}
			}
		}
		else if (key == "NormalMap" && field.value().is_string())
		{
			std::string_view texturePath = field.value().get_string();
			m_normalID = RessourceManager::Get().GetRessourceID<Texture>(std::string(texturePath));
		}
	}

	return true;
}

void Material::Unload()
{
	RessourceManager::Get().UnloadByID<Texture>(m_baseID);
	m_baseID = TOMBSTONE_RESOURCE;

	RessourceManager::Get().UnloadByID<Texture>(m_specularID);
	m_specularID = TOMBSTONE_RESOURCE;

	RessourceManager::Get().UnloadByID<Texture>(m_normalID);
	m_normalID = TOMBSTONE_RESOURCE;
}

bool Material::Add(const std::string& _filePath, IRessource* _ressource)
{
	if (_ressource == nullptr)
	{
		return false;
	}

	Material* material = static_cast<Material*>(_ressource);

	m_baseID = material->GetBase();
	m_baseColor = material->GetBaseColor();

	m_specularID = material->GetSpecular();
	m_specularColor = material->GetSpecularColor();

	m_normalID = material->GetNormal();

	m_smoothness = material->GetSmoothness();

	// Create material file 
	nlohmann::json json;

	json["BaseMap"]["Texture"] = RessourceManager::Get().GetFilePath<Texture>(m_baseID);
	json["BaseMap"]["Color"] = { m_baseColor.x, m_baseColor.y, m_baseColor.z };

	json["SpecularMap"]["Texture"] = RessourceManager::Get().GetFilePath<Texture>(m_specularID);
	json["SpecularMap"]["Color"] = { m_specularColor.x, m_specularColor.y, m_specularColor.z };
	json["SpecularMap"]["Smoothness"] = m_smoothness;

	std::ofstream file(_filePath);
	file << json.dump(4);
	file.close();

	return true;
}

void Material::SetBase(const std::string& _texturePath)
{
	m_baseID = RessourceManager::Get().GetRessourceID<Texture>(_texturePath);
}

void Material::SetBase(const RessourceID& _textureID)
{
	m_baseID = _textureID;
}

RessourceID Material::GetBase() const
{
	return m_baseID;
}

void Material::SetBaseColor(const Vec3f& _color)
{
	m_baseColor = _color;
}

Vec3f Material::GetBaseColor() const
{
	return m_baseColor;
}

void Material::SetSpecular(const std::string& _texturePath)
{
	m_specularID = RessourceManager::Get().GetRessourceID<Texture>(_texturePath);
}

void Material::SetSpecular(const RessourceID& _textureID)
{
	m_specularID = _textureID;
}

RessourceID Material::GetSpecular() const
{
	return m_specularID;
}

void Material::SetSpecularColor(const Vec3f& _color)
{
	m_specularColor = _color;
}

Vec3f Material::GetSpecularColor() const
{
	return m_specularColor;
}

void Material::SetNormal(const std::string& _texturePath)
{
	m_normalID = RessourceManager::Get().GetRessourceID<Texture>(_texturePath);
}

void Material::SetNormal(const RessourceID& _textureID)
{
	m_normalID = _textureID;
}

RessourceID Material::GetNormal() const
{
	return m_normalID;
}

void Material::SetSmoothness(float _smoothness)
{
	m_smoothness = _smoothness;
}

float Material::GetSmoothness()
{
	return m_smoothness;
}
