#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoord;

layout(std140) uniform Matrices
{
	mat4 cam;
};

uniform mat4 uCamMatrix;
uniform mat4 model;
uniform mat3 modelInverse;

out VS_OUT
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Color;
} vsOut;

void main()
{
	vsOut.FragPos = vec3(model * vec4(aPos, 1.f));
	vsOut.Normal = modelInverse * aNormal;
	vsOut.Color = aColor;
	vsOut.TexCoord = mat2(1.0, 0.0, 0.0, -1.0) * aTexCoord;

	gl_Position = uCamMatrix * vec4(vsOut.FragPos, 1.f);
}