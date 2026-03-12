#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 uCamMatrix;

void main()
{
	TexCoords = aPos;
	vec4 pos = uCamMatrix * vec4(aPos, 1.f);
	gl_Position = pos.xyww;
}