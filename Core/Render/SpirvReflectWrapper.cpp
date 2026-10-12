#include "SpirvReflectWrapper.h"

#include <filesystem>
#include <JSON/simdjson.h>

#include "../Logger.h"
#include "../ImGui/imgui_internal.h"

void SpirvReflectWrapper::LoadShader(const std::string& _filePath)
{
	const std::string shaderFileCache = _filePath + ".metadata";
	if (std::filesystem::exists(shaderFileCache))
	{
		ParseJson(shaderFileCache);
	}
}

void SpirvReflectWrapper::ParseJson(const std::string& _filePath)
{
	simdjson::padded_string json;
	if (simdjson::padded_string::load(_filePath).get(json))
	{
		simdjson::ondemand::parser parser;
		simdjson::ondemand::document data;

		if (parser.iterate(json).get(data))
		{
			LOGGER_ERROR("Can't parse the json file !");
		}

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
								simdjson::ondemand::object bindingInfoObject = bindingInfo.value().get_object();
								for (auto bindingInfoField : bindingInfoObject)
								{
									const std::string_view bindingInfoKey = bindingInfoField.unescaped_key();
									if (bindingInfoKey == "Uniform")
									{
										ShaderInfo shaderInfo;

										simdjson::ondemand::object uniformObject = bindingInfoField.value().get_object();
										for (auto uniformField : uniformObject)
										{
											const std::string_view uniformKey = uniformField.unescaped_key();

											if (uniformKey == "Name")
											{
												shaderInfo.name = uniformField.value().get_string().value();
											}
											else if (uniformKey == "UniformType")
											{
												shaderInfo.uniformType = static_cast<UniformType>(uniformField.value().get_int32().value());
											}
											else if (uniformKey == "ShaderStage")
											{
												shaderInfo.shaderStage = static_cast<ShaderStage>(uniformField.value().get_int32().value());
											}
											else if (uniformKey == "Binding")
											{
												shaderInfo.binding = uniformField.value().get_int32();
											}
											else if (uniformKey == "Bindless")
											{
												shaderInfo.bindless = uniformField.value().get_bool();
											}
										}
										shaderInfos.emplace_back(shaderInfo);
									}
								}
							}

							m_shaderInfos.emplace_back(shaderInfos);
						}
					}
				}
			}
		}
	}
}