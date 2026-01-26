#ifndef FRAGMENT_SHADER_DEFAULT__H
#define FRAGMENT_SHADER_DEFAULT__H

#include "../Macro.h"

namespace ENGINE_NAME
{
	const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D uTexture;

void main()
{
    FragColor = texture(uTexture, texCoord);
}
)";
}

#endif