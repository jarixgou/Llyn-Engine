#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;

	vec3 position;
};

void main()
{
	TexCoord = aPos;
	vec4 pos = proj * view * vec4(aPos * 500.f, 1.f);
	gl_Position = pos.xyww;
}