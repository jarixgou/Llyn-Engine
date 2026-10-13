#ifndef SPIRV_REFLECT_WRAPPER__H
#define SPIRV_RELFECT_WRAPPER__H
#include <string>
#include <vector>

#include "Format.h"
#include "ShaderStage.h"
#include "UniformType.h"

struct ShaderInfo
{
	std::string name = "Unknow";
	uint32_t binding = 0;
	UniformType uniformType = UNIFORM_TYPE_CONSTANT;
	ShaderStage shaderStage = SHADER_STAGE_VERTEX;
	bool bindless = false;
};

struct InputAttribute
{
	uint32_t location = 0;
	uint32_t binding = 0;
	uint32_t offset = 0;
	Format format = FORMAT_R32G32B32A32_SFLOAT;
};

class SpirvReflectWrapper
{
public: 
	std::vector<std::vector<ShaderInfo>> m_shaderInfos;
	std::vector<InputAttribute> m_inputAttribute;
public:
	void LoadShader(const std::string& _filePath);
private:
	void ParseJson(const std::string& _filePath);
	void LoadReflect(const std::string& _filePath);

	uint32_t CalculateOffset(Format _format);
};

#endif