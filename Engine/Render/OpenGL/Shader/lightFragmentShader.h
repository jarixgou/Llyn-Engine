#ifndef LIGHT_FRAGMENT_SHADER__H
#define LIGHT_FRAGMENT_SHADER__H



namespace Llyn
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