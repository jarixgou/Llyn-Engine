#ifndef VERTEX_SHADER_DEFAULT__H
#define VERTEX_SHADER_DEFAULT__H

#include "../Macro.h"

namespace ENGINE_NAME
{
	const char* vertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 uCamMatrix;
uniform mat4 model;
uniform mat3 modelInverse;

void main()
{
	crntPos = vec3(model * vec4(aPos, 1.f));
	Normal = modelInverse * aNormal;
	color = aColor;
	texCoord = aTexCoord;

	gl_Position = uCamMatrix * vec4(crntPos, 1.f);
}
)";
}

#endif