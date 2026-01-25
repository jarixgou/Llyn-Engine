#ifndef FRAGMENT_SHADER_DEFAULT__H
#define FRAGMENT_SHADER_DEFAULT__H

namespace Llyn
{
	const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

in vec3 color;

void main()
{
    FragColor = vec4(color, 1.0f);
}
)";
}

#endif