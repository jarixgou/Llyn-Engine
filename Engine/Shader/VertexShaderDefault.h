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
layout(location = 3) in vec3 aNormal;

out vec3 color;
out vec2 texCoord;
out vec3 Normal;
out vec3 crntPos;

uniform mat4 uCamMatrix;
uniform mat4 model;

void main()
{
	crntPos = vec3(model * vec4(aPos, 1.f));
    gl_Position = uCamMatrix * vec4(crntPos, 1.f);
	color = aColor;
	texCoord = aTexCoord;
	Normal = mat3(transpose(inverse(model))) * aNormal;
}
)";
}

#endif