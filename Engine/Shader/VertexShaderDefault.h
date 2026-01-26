#ifndef VERTEX_SHADER_DEFAULT__H
#define VERTEX_SHADER_DEFAULT__H

#include "../Macro.h"

namespace ENGINE_NAME
{
	const char* vertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 color;
out vec2 texCoord;

uniform mat4 uCamMatrix;

void main()
{
    gl_Position = uCamMatrix * vec4(aPos, 1.f);
	color = aColor;
	texCoord = aTexCoord;
}
)";
}

#endif