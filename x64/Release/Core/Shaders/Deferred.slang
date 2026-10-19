struct VSInput
{
    float2 pos;
    float2 uv;
}

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv;
}

struct DirLight
{
    float3 dir;
    float pad0;

    float3 ambient;
    float pad1;

    float3 diffuse;
    float pad2;

    float3 specular;
    float pad3;
}

struct PointLight
{
    float3 pos;
    float pad0;

    float3 ambient;
    float pad1;

    float3 diffuse;
    float pad2;

    float3 specular;
    float pad3;

    float constant;
    float linear;
    float quadratic;

    float radius;
}

struct SpotLight
{
    float3 pos;
    float pad0;

    float3 dir;
    float pad1;

    float3 ambient;
    float pad2;

    float3 diffuse;
    float pad3;

    float3 specular;

    float cutOff;
    float outerCutOff;
}

struct LightData
{
    uint32_t nbDirLight;
    uint32_t nbPointLight;
    uint32_t nbSpotLight;
}

[vk::binding(0, 0)]
Sampler2D posTex;

[vk::binding(1, 0)]
Sampler2D normalTex;

[vk::binding(2, 0)]
Sampler2D albedoTex;

[vk::binding(3, 0)]
Sampler2D specular2D;

[vk::binding(0, 1)]
StructuredBuffer<DirLight> dirLights;

[vk::binding(1, 1)]
StructuredBuffer<PointLight> pointLights;

[vk::binding(2, 1)]
StructuredBuffer<SpotLight> spotLights;

[vk::binding(3, 1)]
ConstantBuffer<LightData> lightData;

[vk::push_constant]
float3 camPos;

[shader("vertex")]
VSOutput vertMain(VSInput _vsInput)
{
    VSOutput output;

    output.pos = float4(_vsInput.pos, 0.0, 1.0);
    output.uv = _vsInput.uv;

    return output;
}



[shader("fragment")]
float4 fragMain(VSOutput _vsOutput) : SV_Target0
{
    float4 finalColor = float4(0.0);

    for (uint i = 0; i < lightData.nbDirLight; i++)
    {

    }
}