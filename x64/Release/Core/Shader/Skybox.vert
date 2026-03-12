#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 uCamMatrix;

void main()
{
	TexCoord = aPos;
	vec4 pos = uCamMatrix * vec4(aPos * 500.f, 1.f);
	gl_Position = pos.xyww;
}