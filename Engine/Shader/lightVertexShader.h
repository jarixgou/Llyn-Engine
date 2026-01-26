#ifndef LIGHT_VERTEX_SHADER__H
#define LIGHT_VERTEX_SHADER__H

#include "../Macro.h"

namespace ENGINE_NAME
{
	const char* lightVertexShader = R"(
#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 uCamMatrix;

void main()
{
	gl_Position = uCamMatrix * model * vec4(aPos, 1.f);
}
)";
}

#endif