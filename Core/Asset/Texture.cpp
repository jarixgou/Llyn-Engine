#include "Texture.h"

#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>
#include <JSON/simdjson.h>
#include <JSON/json.hpp>

#include "../Render/Vulkan/VK_Texture.h"
#include "../Render/Vulkan/VK_TextureInfo.h"
#include "../Vector/Vec2.h"

Texture::~Texture()
{
	
}

bool Texture::Load(const std::string& _filePath)
{
	int texWidth = 0;
	int texHeight = 0;
	int texChannels = 0;

	stbi_uc* pixels = stbi_load(_filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (pixels == nullptr)
	{
		return false;
	}

	// Load texture config
	LoadConfig(_filePath);

	VK_texture = new VK_Texture;

	VK_TextureInfo textureInfo{};
	textureInfo.config = m_config;
	textureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	textureInfo.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	textureInfo.textureSize = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight) };
	textureInfo.data = pixels;

	VK_texture->Init(textureInfo);

	return true;
}

void Texture::Unload()
{
	VK_texture->Cleanup();
	delete VK_texture;
	VK_texture = nullptr;
}

bool Texture::Add(const std::string& _filePath, IRessource* _ressource)
{

	return false;
}

VK_Texture* Texture::GetVKTexture()
{
	return VK_texture;
}

void Texture::LoadConfig(const std::string& _filePath)
{
	std::string metaDataPath = _filePath + ".metadata";


	if (std::filesystem::exists(metaDataPath))
	{
		simdjson::padded_string json;
		if (simdjson::padded_string::load(metaDataPath).get(json))
		{
			simdjson::ondemand::parser parser;
			simdjson::ondemand::document data;

			if (parser.iterate(json).get(data))
			{
				// print error with logger
			}

			simdjson::ondemand::object root = data.get_object();
			for (auto field : root)
			{
				std::string_view key = field.unescaped_key();
				if (key == "Format" && field.value().is_integer())
				{
					m_config.format = static_cast<ImageFormat>(field.value().get_int32().value());
				}
				if (key == "Type" && field.value().is_integer())
				{
					m_config.type  = static_cast<ImageType>(field.value().get_int32().value());
				}
				else if (key == "sRGB" && field.value().is_integer())
				{
					m_config.sRGB = field.value().get_bool();
				}
				else if (key == "WrapMode" && field.value().is_integer())
				{
					m_config.wrapMode = static_cast<WrapMode>(field.value().get_int32().value());
				}
				else if (key == "FilterMode" && field.value().is_integer())
				{
					m_config.filter = static_cast<ImageFilter>(field.value().get_int32().value());
				}
				else if (key == "Mipmap" && field.value().is_integer())
				{
					m_config.mipmap = field.value().get_bool();
				}
				else if (key == "MipmapFilter" && field.value().is_integer())
				{
					m_config.mipMapFilter = static_cast<MipMapFilter>(field.value().get_int32().value());
				}
			}
		}
	}
	else
	{
		m_config = TextureConfig{};
		nlohmann::json json;

		json["Format"] = m_config.format;
		json["Type"] = m_config.type;
		json["sRGB"] = m_config.sRGB;
		json["WrapMode"] = m_config.wrapMode;
		json["ImageFilter"] = m_config.filter;
		json["Mipmap"] = m_config.mipmap;
		json["MipmapFilter"] = m_config.mipMapFilter;

		std::ofstream file(metaDataPath);
		file << json.dump(4);
		file.close();
	}
}
