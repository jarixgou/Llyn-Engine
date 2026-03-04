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
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat3 modelInverse;

void main()
{
	crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.f));
	Normal = modelInverse * mat3(rotation) * aNormal;
	color = aColor;
	texCoord = mat2(1.0, 0.0, 0.0, -1.0) *aTexCoord;

	gl_Position = uCamMatrix * vec4(crntPos, 1.f);
}