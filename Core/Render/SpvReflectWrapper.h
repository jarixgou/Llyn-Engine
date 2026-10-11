#ifndef SPV_REFLECT_WRAPPER__H
#define SPV_REFLECT_WRAPPER__H
#include <string>
#include <vector>

#include "UniformType.h"
#include "ShaderStage.h"

struct ShaderInfo
{
	std::string name = "Unknow";
	uint32_t binding = 0;
	UniformType uniformType = UNIFORM_TYPE_CONSTANT;
	ShaderStage shaderStage = SHADER_STAGE_VERTEX;
	bool bindless = false;

	uint32_t count = 1;
};

class SpvReflectWrapper
{
public:
	std::vector<std::vector<ShaderInfo>> m_shaderInfos;
public:
	void LoadReflect(const std::string& _filePath);
private:
	void ParseJson(const std::string& _filePath);
};

#endif