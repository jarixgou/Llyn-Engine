#include "SpirvReflectWrapper.h"

#include <filesystem>
#include <JSON/simdjson.h>

#include "../LlynCore.h"
#include "../Logger.h"
#include "../ImGui/imgui_internal.h"
#include "../SPIRV-Reflect/spirv_reflect.h"
#include "../Utils/Utils.h"

void SpirvReflectWrapper::LoadShader(const std::string& _filePath)
{
	const std::string shaderFileCache = _filePath + ".metadata";
	if (std::filesystem::exists(shaderFileCache))
	{
		ParseJson(shaderFileCache);
	}
	else
	{
		LoadReflect(_filePath);
	}
}

void SpirvReflectWrapper::ParseJson(const std::string& _filePath)
{
	simdjson::padded_string json;
	if (simdjson::padded_string::load(_filePath).get(json))
	{
		LOGGER_ERROR("Failed to load shader json !");
		return;
	}

	simdjson::ondemand::parser parser;
	simdjson::ondemand::document data;

	if (parser.iterate(json).get(data))
	{
		LOGGER_ERROR("Failed to parse the json file !");
		return;
	}

	try
	{
		simdjson::ondemand::object root = data.get_object();
		for (auto field : root)
		{
			const std::string_view key = field.unescaped_key();
			if (key == "Sets")
			{
				simdjson::ondemand::array setsArray = field.value().get_array();
				for (auto set : setsArray)
				{
					simdjson::ondemand::object setObject = set.value().get_object();
					for (auto setField : setObject)
					{
						const std::string_view setKey = setField.unescaped_key();
						if (setKey == "BindingInfos")
						{
							std::vector<ShaderInfo> shaderInfos;

							simdjson::ondemand::array bindingInfosArray = setField.value().get_array();
							for (auto bindingInfo : bindingInfosArray)
							{
								ShaderInfo shaderInfo;

								simdjson::ondemand::object bindingInfoObject = bindingInfo.value().get_object();
								for (auto bindingInfoField : bindingInfoObject)
								{
									const std::string_view bindingInfoKey = bindingInfoField.unescaped_key();

									if (bindingInfoKey == "Name")
									{
										shaderInfo.name = bindingInfoField.value().get_string().value();
									}
									else if (bindingInfoKey == "Binding")
									{
										shaderInfo.binding = bindingInfoField.value().get_int32().value();
									}
									else if (bindingInfoKey == "UniformType")
									{
										shaderInfo.uniformType = static_cast<UniformType>(bindingInfoField.value().get_int32().value());
									}
									else if (bindingInfoKey == "ShaderStage")
									{
										shaderInfo.shaderStage = static_cast<ShaderStage>(bindingInfoField.value().get_int32().value());
									}
									else if (bindingInfoKey == "Bindless")
									{
										shaderInfo.bindless = bindingInfoField.value().get_bool();
									}
								}
								shaderInfos.emplace_back(std::move(shaderInfo));
							}

							m_shaderInfos.emplace_back(std::move(shaderInfos));
						}
					}
				}
			}
		}
	}
	catch (const simdjson::simdjson_error& e)
	{
		LOGGER_ERROR("Invalid shader json : " + std::string(e.what()));
	}
}

void SpirvReflectWrapper::LoadReflect(const std::string& _filePath)
{
	std::vector<char> fileContent = Utils::ReadFile(_filePath);

	SpvReflectShaderModule module{};
	SPV_CHECK(spvReflectCreateShaderModule(fileContent.size(), fileContent.data(), &module), 
		"Failed to create spv reflect shader module ! ");

	uint32_t count = 0;

	SPV_CHECK(spvReflectEnumerateDescriptorSets(&module, &count, nullptr),
		"Failed to enumerate descriptor sets !");
	std::vector<SpvReflectDescriptorSet*> sets(count);
	SPV_CHECK(spvReflectEnumerateDescriptorSets(&module, &count, sets.data()),
		"Failed to enumerate descriptor sets !");


}
