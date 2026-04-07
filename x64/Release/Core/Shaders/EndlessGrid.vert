#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

layout(std140) uniform Matrix
{
	mat4 cam;
};

uniform mat4 uCamMatrix;
uniform vec3 uCamPos;

out VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec3 Color;
	vec2 TexCoord;
} vsOut;

void main()
{
	mat4 model = mat4(1.f);
	vsOut.FragPos = vec3(model * vec4(aPos, 1.f));
	vsOut.FragPos *= 100.f;
	vsOut.FragPos.xz += uCamPos.xz;

	vsOut.Normal = transpose(inverse(mat3(model))) * aNormal;
	vsOut.Color = aColor;
	vsOut.TexCoord = aTexCoord;

	gl_Position = uCamMatrix * vec4(vsOut.FragPos, 1.f);
}