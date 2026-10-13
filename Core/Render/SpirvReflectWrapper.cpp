#include "SpirvReflectWrapper.h"

#include <filesystem>
#include <JSON/simdjson.h>

#include "../LlynCore.h"
#include "../Logger.h"
#include "../ImGui/imgui_internal.h"
#include "../SPIRV-Reflect/spirv_reflect.h"
#include "../Utils/Utils.h"

Format GetFormat(SpvReflectFormat _format);

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

	for (uint32_t i = 0; i < module.entry_point_count; ++i)
	{
		const SpvReflectEntryPoint entryPoint = module.entry_points[i];

		// Get and fin Vertex Entry
		if (entryPoint.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
		{
			uint32_t count = 0;

			SPV_CHECK(spvReflectEnumerateEntryPointInputVariables(&module, entryPoint.name, &count, nullptr),
				"Failed to enumerate entry point input variable !");
			std::vector<SpvReflectInterfaceVariable*> vertexInput(count);
			SPV_CHECK(spvReflectEnumerateEntryPointInputVariables(&module, entryPoint.name, &count, vertexInput.data()),
				"Failed to enumerate entry point input variable");

			m_inputAttribute.resize(count);
			for (uint32_t j = 0; j < count; ++j)
			{
				m_inputAttribute[j].location = vertexInput[j]->location;
				m_inputAttribute[j].binding = 0;
				m_inputAttribute[j].format = GetFormat(vertexInput[j]->format);
			}

			// Bubble sort to calculate the offset
			for (uint32_t j = 0; j < count - 1; ++j)
			{
				bool swapped = false;
				for (uint32_t k = 0; k < count - j - 1; ++k)
				{
					if (m_inputAttribute[k].location > m_inputAttribute[k + 1].location)
					{
						InputAttribute inputAttributeCopy = m_inputAttribute[k];
						m_inputAttribute[k] = m_inputAttribute[k + 1];
						m_inputAttribute[k + 1] = inputAttributeCopy;

						swapped = true;
					}
				}

				if (!swapped)
				{
					j = count;
				}
			}

			// Calculate the offset
			uint32_t offset = 0;
			for (int j = 0; j < count; ++j)
			{
				m_inputAttribute[j].offset = offset;

				offset += CalculateOffset(m_inputAttribute[j].format);
			}
		}

		// Get and set output image 
		if (entryPoint.shader_stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)
		{
			uint32_t count = 0;

			SPV_CHECK(spvReflectEnumerateEntryPointOutputVariables(&module, entryPoint.name, &count, nullptr),
				"Failed to enumerate entry point output variables ! ");
			std::vector<SpvReflectInterfaceVariable*> outputImage(count);
			SPV_CHECK(spvReflectEnumerateEntryPointOutputVariables(&module, entryPoint.name, &count, outputImage.data()),
				"Failed to enumerate entry point output variables ! ");

			for (uint32_t j = 0; j < count; ++j)
			{
				
			}
		}
	}
}

uint32_t SpirvReflectWrapper::CalculateOffset(Format _format)
{
	switch (_format)
	{
	case FORMAT_R8_SINT:				return sizeof(int8_t);
	case FORMAT_R8G8_SINT:				return sizeof(int8_t) * 2;
	case FORMAT_R8G8B8_SINT:			return sizeof(int8_t) * 3;
	case FORMAT_R8G8B8A8_SINT:			return sizeof(int8_t) * 4;
	case FORMAT_R8_UINT:				return sizeof(uint8_t);
	case FORMAT_R8G8_UINT:				return sizeof(uint8_t) * 2;
	case FORMAT_R8G8B8_UINT:			return sizeof(uint8_t) * 3;
	case FORMAT_R8G8B8A8_UINT:			return sizeof(uint8_t) * 4;
	case FORMAT_R16_SINT:				return sizeof(int16_t);
	case FORMAT_R16G16_SINT:			return sizeof(int16_t) * 2;
	case FORMAT_R16G16B16_SINT:			return sizeof(int16_t) * 3;
	case FORMAT_R16G16B16A16_SINT:		return sizeof(int16_t) * 4;
	case FORMAT_R16_UINT:				return sizeof(uint16_t);
	case FORMAT_R16G16_UINT:			return sizeof(uint16_t) * 2;
	case FORMAT_R16G16B16_UINT:			return sizeof(uint16_t) * 3;
	case FORMAT_R16G16B16A16_UINT:		return sizeof(uint16_t) * 4;
	case FORMAT_R32_SINT:				return sizeof(int32_t);
	case FORMAT_R32G32_SINT:			return sizeof(int32_t) * 2;
	case FORMAT_R32G32B32_SINT:			return sizeof(int32_t) * 3;
	case FORMAT_R32G32B32A32_SINT:		return sizeof(int32_t) * 4;
	case FORMAT_R32_UINT:				return sizeof(uint32_t);
	case FORMAT_R32G32_UINT:			return sizeof(uint32_t) * 2;
	case FORMAT_R32G32B32_UINT:			return sizeof(uint32_t) * 3;
	case FORMAT_R32G32B32A32_UINT:		return sizeof(uint32_t) * 4;
	case FORMAT_R32_SFLOAT:				return sizeof(float);
	case FORMAT_R32G32_SFLOAT:			return sizeof(float) * 2;
	case FORMAT_R32G32B32_SFLOAT:		return sizeof(float) * 3;
	case FORMAT_R32G32B32A32_SFLOAT:	return sizeof(float) * 4;
	case FORMAT_MAX:
		LOGGER_ERROR("Invalid format to calculate offset ! ");
		break;
	}

	LOGGER_ERROR("Can't find the right enum to calculate the offset ! ");
	return sizeof(float) * 4;
}

Format GetFormat(SpvReflectFormat _format)
{
	switch (_format)
	{
	case SPV_REFLECT_FORMAT_UNDEFINED:				return FORMAT_R32G32B32A32_SFLOAT;
	case SPV_REFLECT_FORMAT_R16_UINT:				return FORMAT_R16_UINT;
	case SPV_REFLECT_FORMAT_R16_SINT:				return FORMAT_R16_SINT;
	case SPV_REFLECT_FORMAT_R16_SFLOAT:				return FORMAT_R16_SFLOAT;
	case SPV_REFLECT_FORMAT_R16G16_UINT:			return FORMAT_R16G16_UINT;
	case SPV_REFLECT_FORMAT_R16G16_SINT:			return FORMAT_R16G16_SINT;
	case SPV_REFLECT_FORMAT_R16G16_SFLOAT:			return FORMAT_R16G16_SFLOAT;
	case SPV_REFLECT_FORMAT_R16G16B16_UINT:			return FORMAT_R16G16B16_UINT;
	case SPV_REFLECT_FORMAT_R16G16B16_SINT:			return FORMAT_R16G16B16_SINT;
	case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:		return FORMAT_R16G16B16_SFLOAT;
	case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:		return FORMAT_R16G16B16A16_UINT;
	case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:		return FORMAT_R16G16B16A16_SINT;
	case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:	return FORMAT_R16G16B16A16_SFLOAT;
	case SPV_REFLECT_FORMAT_R32_UINT:				return FORMAT_R32_UINT;
	case SPV_REFLECT_FORMAT_R32_SINT:				return FORMAT_R32_SINT;
	case SPV_REFLECT_FORMAT_R32_SFLOAT:				return FORMAT_R32_SFLOAT;
	case SPV_REFLECT_FORMAT_R32G32_UINT:			return FORMAT_R32G32_UINT;
	case SPV_REFLECT_FORMAT_R32G32_SINT:			return FORMAT_R32G32_SINT;
	case SPV_REFLECT_FORMAT_R32G32_SFLOAT:			return FORMAT_R32G32_SFLOAT;
	case SPV_REFLECT_FORMAT_R32G32B32_UINT:			return FORMAT_R32G32B32_UINT;
	case SPV_REFLECT_FORMAT_R32G32B32_SINT:			return FORMAT_R32G32B32_SINT;
	case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:		return FORMAT_R32G32B32_SFLOAT;
	case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:		return FORMAT_R32G32B32A32_UINT;
	case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:		return FORMAT_R32G32B32A32_SINT;
	case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:	return FORMAT_R32G32B32A32_SFLOAT;
	}

	return FORMAT_R32G32B32A32_SFLOAT;
}
