#version 460

layout(location = 0) in VS_OUT
{
    vec3 fragPos;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
} vsOut;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraBuffer
{
    mat4 view;
    mat4 proj;
} camera;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main()
{
    outColor = texture(texSampler, vsOut.texCoord);
}