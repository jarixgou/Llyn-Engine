#include "SpirvReflectWrapper.h"

#include <filesystem>
#include <JSON/simdjson.h>

#include "../LlynCore.h"
#include "../Logger.h"
#include "../ImGui/imgui_internal.h"
#include "../SPIRV-Reflect/spirv_reflect.h"
#include "../Utils/Utils.h"

Format GetFormat(SpvReflectFormat _format);
UniformType GetUniformType(const SpvReflectDescriptorType& _type);
ShaderStage GetShaderStage(const SpvReflectShaderStageFlagBits& _type);

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

							//m_shaderInfos.emplace_back(std::move(shaderInfos));
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

			for (uint32_t j = 0; j < count; ++j)
			{
				if (!(vertexInput[j]->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN))
				{
					InputAttribute inputAttribute;
					inputAttribute.location = vertexInput[j]->location;
					inputAttribute.binding = 0;
					inputAttribute.format = GetFormat(vertexInput[j]->format);
					m_inputAttribute.emplace_back(inputAttribute);
				}
			}

			// Bubble sort to calculate the offset
			for (int j = 0; j < static_cast<int>(m_inputAttribute.size()) - 1; ++j)
			{
				bool swapped = false;
				for (int k = 0; k < static_cast<int>(m_inputAttribute.size()) - j - 1; ++k)
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
					j = m_inputAttribute.size();
				}
			}

			// Calculate the offset
			uint32_t offset = 0;
			for (size_t j = 0; j < m_inputAttribute.size(); ++j)
			{
				m_inputAttribute[j].offset = offset;

				offset += CalculateOffset(m_inputAttribute[j].format);
				m_attributeInputSizeByte += CalculateOffset(m_inputAttribute[j].format);
			}

			count = 0;
			SPV_CHECK(spvReflectEnumerateEntryPointDescriptorSets(&module, entryPoint.name, &count, nullptr),
				"Failed to enumerate descriptor sets ! ");
			std::vector<SpvReflectDescriptorSet*> descriptorSets(count);
			SPV_CHECK(spvReflectEnumerateEntryPointDescriptorSets(&module, entryPoint.name, &count, descriptorSets.data()),
				"Failed to enumerate descriptor sets ! ");

			for (uint32_t j = 0; j < count; ++j)
			{
				SetInfo* setInfo = nullptr;

				for (size_t k = 0; k < m_setInfos.size(); ++k)
				{
					if (m_setInfos[k].set == descriptorSets[j]->set)
					{
						setInfo = &m_setInfos[k];
						k = m_setInfos.size();
					}
				}

				if (setInfo == nullptr)
				{
					setInfo = &m_setInfos.emplace_back();
					setInfo->set = descriptorSets[j]->set;
				}

				for (uint32_t k = 0; k < descriptorSets[j]->binding_count; ++k)
				{
					ShaderInfo shaderInfo;

					shaderInfo.name = descriptorSets[j]->bindings[k]->name;
					shaderInfo.binding = descriptorSets[j]->bindings[k]->binding;
					shaderInfo.uniformType = GetUniformType(descriptorSets[j]->bindings[k]->descriptor_type);
					shaderInfo.shaderStage = GetShaderStage(entryPoint.shader_stage);

					if (descriptorSets[j]->bindings[k]->array.dims_count > 0 &&
						descriptorSets[j]->bindings[k]->array.dims[0] == SPV_REFLECT_ARRAY_DIM_RUNTIME)
					{
						shaderInfo.bindless = true;
					}

					setInfo->shaderInfos.emplace_back(std::move(shaderInfo));
				}
			}

			count = 0;
			SPV_CHECK(spvReflectEnumerateEntryPointPushConstantBlocks(&module, entryPoint.name, &count, nullptr),
				"Failed to enumerate push constant blocks ! ");
			std::vector<SpvReflectBlockVariable*> pushConstants(count);
			SPV_CHECK(spvReflectEnumerateEntryPointPushConstantBlocks(&module, entryPoint.name, &count, pushConstants.data()),
				"Failed to enumerate push constant blocks ! ");

			for (uint32_t j = 0; j < count; ++j)
			{
				PushConstant pushConstant;
				pushConstant.name = pushConstants[j]->name;
				pushConstant.shaderStage = GetShaderStage(entryPoint.shader_stage);
				pushConstant.size = pushConstants[j]->size;
				pushConstant.offset = pushConstants[j]->offset;
			}
		}

		// Get the number of output image
		if (entryPoint.shader_stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)
		{
			SPV_CHECK(spvReflectEnumerateEntryPointOutputVariables(&module, entryPoint.name, &m_imageOutputCount, nullptr),
				"Failed to enumerate entry point output variables ! ");

			uint32_t count = 0;
			SPV_CHECK(spvReflectEnumerateEntryPointDescriptorSets(&module, entryPoint.name, &count, nullptr),
				"Failed to enumerate descriptor sets ! ");
			std::vector<SpvReflectDescriptorSet*> descriptorSets(count);
			SPV_CHECK(spvReflectEnumerateEntryPointDescriptorSets(&module, entryPoint.name, &count, descriptorSets.data()),
				"Failed to enumerate descriptor sets ! ");

			for (uint32_t j = 0; j < count; ++j)
			{
				SetInfo* setInfo = nullptr;

				for (size_t k = 0; k < m_setInfos.size(); ++k)
				{
					if (m_setInfos[k].set == descriptorSets[j]->set)
					{
						setInfo = &m_setInfos[k];
						k = m_setInfos.size();
					}
				}

				if (setInfo == nullptr)
				{
					setInfo = &m_setInfos.emplace_back();
					setInfo->set = descriptorSets[j]->set;
				}

				for (uint32_t k = 0; k < descriptorSets[j]->binding_count; ++k)
				{
					ShaderInfo shaderInfo;

					shaderInfo.name = descriptorSets[j]->bindings[k]->name;
					shaderInfo.binding = descriptorSets[j]->bindings[k]->binding;
					shaderInfo.uniformType = GetUniformType(descriptorSets[j]->bindings[k]->descriptor_type);
					shaderInfo.shaderStage = GetShaderStage(entryPoint.shader_stage);

					if (descriptorSets[j]->bindings[k]->array.dims_count > 0 &&
						descriptorSets[j]->bindings[k]->array.dims[0] == SPV_REFLECT_ARRAY_DIM_RUNTIME)
					{
						shaderInfo.bindless = true;
					}

					setInfo->shaderInfos.emplace_back(std::move(shaderInfo));
				}
			}

			count = 0;
			SPV_CHECK(spvReflectEnumerateEntryPointPushConstantBlocks(&module, entryPoint.name, &count, nullptr),
				"Failed to enumerate push constant blocks ! ");
			std::vector<SpvReflectBlockVariable*> pushConstants(count);
			SPV_CHECK(spvReflectEnumerateEntryPointPushConstantBlocks(&module, entryPoint.name, &count, pushConstants.data()),
				"Failed to enumerate push constant blocks ! ");

			for (uint32_t j = 0; j < count; ++j)
			{
				PushConstant pushConstant;
				pushConstant.name = pushConstants[j]->name;
				pushConstant.shaderStage = GetShaderStage(entryPoint.shader_stage);
				pushConstant.size = pushConstants[j]->size;
				pushConstant.offset = pushConstants[j]->offset;

				m_pushConstants.emplace_back(std::move(pushConstant));
			}
		}
	}


	// Bubble sort set info
	for (size_t i = 0; i < m_setInfos.size() - 1; ++i)
	{
		bool swapped = false;

		for (size_t j = 0; j < m_setInfos.size() - i - 1; ++j)
		{
			if (m_setInfos[j].set > m_setInfos[j + 1].set)
			{
				SetInfo setInfo = m_setInfos[j];
				m_setInfos[j] = m_setInfos[j + 1];
				m_setInfos[j + 1] = setInfo;

				swapped = true;
			}
		}

		if (!swapped)
		{
			i = m_setInfos.size();
		}
	}

	spvReflectDestroyShaderModule(&module);
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
	default: LOGGER_ERROR("Unsupported format ! ");
	}

	return FORMAT_R32G32B32A32_SFLOAT;
}

UniformType GetUniformType(const SpvReflectDescriptorType& _type)
{
	switch (_type)
	{
	case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:	return UNIFORM_TYPE_COMBINED_IMAGE_SAMPLER;
	case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:			return UNIFORM_TYPE_CONSTANT;
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:			return UNIFORM_TYPE_STORAGE;
	default: LOGGER_ERROR("Unsupported uniform type ! ");
	}

	return UNIFORM_TYPE_CONSTANT;
}

ShaderStage GetShaderStage(const SpvReflectShaderStageFlagBits& _type)
{
	switch (_type)
	{
	case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:		return SHADER_STAGE_VERTEX;
	case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:		return SHADER_STAGE_FRAGMENT;
	case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:		return SHADER_STAGE_COMPUTE;
	default: LOGGER_ERROR("Unsupported shader stage ! ");
	}

	return SHADER_STAGE_VERTEX;
}