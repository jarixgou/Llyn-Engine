#ifndef LIGHT_VERTEX_SHADER__H
#define LIGHT_VERTEX_SHADER__H

#include "../Macro.h"

namespace ENGINE_NAME
{
	const char* lightVertexShader = R"(
#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 uCamMatrix;

uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
void main()
{
	gl_Position = uCamMatrix * model * translation * -rotation * scale * vec4(aPos, 1.f);
}
)";
}

#endif