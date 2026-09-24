#version 460 

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out VS_OUT
{
    vec3 fragPos;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
} vsOut;

layout(set = 0, binding = 0) uniform CameraBuffer
{
    mat4 view;
    mat4 proj;
} camera;

layout(push_constant) uniform ModelInfoConstant
{
   mat4 model;
   mat4 inverseModel;
} modelInfo;

void main()
{
    vsOut.fragPos = vec3(modelInfo.model * vec4(inPos, 1.0f));
    vsOut.normal = mat3(modelInfo.inverseModel) * inNormal;
    vsOut.color = inColor;
    vsOut.texCoord = inTexCoord;

    gl_Position = camera.proj * camera.view * vec4(vsOut.fragPos, 1.0f);
}