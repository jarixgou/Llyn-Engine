#ifndef SPIRV_REFLECT_WRAPPER__H
#define SPIRV_RELFECT_WRAPPER__H
#include <string>
#include <vector>

#include "ShaderStage.h"
#include "UniformType.h"

struct ShaderInfo
{
	std::string name = "Unknow";
	UniformType uniformType = UNIFORM_TYPE_CONSTANT;
	ShaderStage shaderStage = SHADER_STAGE_VERTEX;
	uint32_t binding = 0;
	bool bindless = false;
};

class SpirvReflectWrapper
{
public: 
	std::vector<std::vector<ShaderInfo>> m_shaderInfos;
public:
	void LoadShader(const std::string& _filePath);
private:
	void ParseJson(const std::string& _filePath);
};

#endif