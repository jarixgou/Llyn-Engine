#include "SpvReflectWrapper.h"

#include <filesystem>
#include <JSON/simdjson.h>

#include "../SPIRV-Reflect/spirv_reflect.h"

void SpvReflectWrapper::LoadReflect(const std::string& _filePath)
{
	std::string shaderFileCache = _filePath + ".metadata";

	if (std::filesystem::exists(shaderFileCache))
	{
		ParseJson(shaderFileCache);
	}

	SpvReflectShaderModule reflectShader;
}

void SpvReflectWrapper::ParseJson(const std::string& _filePath)
{
	simdjson::padded_string json;
	if (simdjson::padded_string::load(_filePath).get(json))
	{
		simdjson::ondemand::parser parser;
		simdjson::ondemand::document data;

		if (parser.iterate(json).get(data))
		{
			// TODO: print error with logger
		}

		simdjson::ondemand::object root = data.get_object();
		for (auto field : root)
		{
			std::string_view key = field.unescaped_key();
			if (key == "Sets" && !field.value().is_null())
			{
				simdjson::ondemand::array setsArray = field.value().get_array();
				for (auto set : setsArray)
				{
					simdjson::ondemand::object setObject = set.get_object();
					for (auto binding : setObject)
					{
						
					}
				}
			}
		}
	}
}
