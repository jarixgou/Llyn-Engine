#ifndef LIGHT_FRAGMENT_SHADER__H
#define LIGHT_FRAGMENT_SHADER__H

#include "../Macro.h"

namespace ENGINE_NAME
{
	const char* lightFragmentShader = R"(
#version 460 core

out vec4 FragColor;

uniform vec4 lightColor;

void main()
{
	FragColor = lightColor;
}
)";
}

#endif