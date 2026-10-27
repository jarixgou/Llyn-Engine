#define TOMBSTONE_ID 65535

struct VertexData
{
    float3 pos;
    float pad0;

    float3 normal;
    float pad1;

    float4 tangent;

    float3 color;
    float pad4;

    float2 uv0;
    float2 uv1;
}

struct Matrices
{
    float4x4 mvp;
    float4x4 localPos;
    float4x4 normal;
}

enum MaterialMode
{
    SPECULAR,
    METALLIC
}

struct Material
{
    MaterialMode mode;
    uint16_t albedoID;
    uint16_t specularID;
    uint16_t metallicID;
    uint16_t normalID;
    uint16_t emissiveID;
    uint16_t aoID;

    float3 albedoColor;
    float pad0;

    float3 specularColor;
    float smoothness;

    float3 emissiveColor;
    float pad1;
}

struct MetaData
{
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t vertexOffset;

    uint16_t materialID;
}

struct VSOutput
{
    float4 pos : SV_Position;

    float3 worldPos;

    float3 color;

    float3 normal;

    float3x3 TBN;

    float3 barycentric;

    float2 uv0;
    float2 uv1;

    nointerpolation uint materialID;
}

struct FGOutput
{
    float4 pos : SV_Target0;
    float4 normal : SV_Target1;
    float4 albedo : SV_Target2;
    float4 specular : SV_Target3;
    float4 emissive : SV_Target4;
    float2 ao : SV_Target5;
}

// Bindless
[vk::binding(0, 0)]
Sampler2D textures[];

// Core
[vk::binding(0, 1)]
StructuredBuffer<VertexData> vertices;

[vk::binding(1, 1)]
StructuredBuffer<uint32_t> indices;

[vk::binding(2, 1)]
StructuredBuffer<Material> materials;

// Scene
[vk::binding(0, 2)]
StructuredBuffer<Matrices> matrices;

[vk::binding(1, 2)]
StructuredBuffer<MetaData> metaDatas;

[shader("vertex")]
VSOutput vertMain(uint _vertexID: SV_VertexID, uint _instance: SV_StartInstanceLocation)
{
    const MetaData metaData = metaDatas[_instance];
    const Matrices matrix = matrices[_instance];

    const uint index = indices[metaData.indexOffset + _vertexID];

    const VertexData vertexData = vertices[metaData.vertexOffset + index];

    VSOutput output;
    output.pos = mul(matrix.mvp, float4(vertexData.pos, 1.0f));
    output.worldPos = mul(matrix.localPos, float4(vertexData.pos, 1.0f)).xyz;
    output.normal = normalize(mul((float3x3)matrix.normal, vertexData.normal));

    float3 T = normalize(mul((float3x3)matrix.localPos, vertexData.tangent.xyz));
    T = normalize(T - dot(T, output.normal) * output.normal);

    const float3 B = cross(output.normal, T) * vertexData.tangent.w;

    output.TBN = float3x3(T, B, output.normal);

    output.color = vertexData.color;

    output.uv0 = vertexData.uv0;
    output.uv1 = vertexData.uv1;

    output.materialID = metaData.materialID;

    const uint vertexMod = _vertexID % 3;
    output.barycentric =
        float3(vertexMod == 0 ? 1.0f : 0.0f, vertexMod == 1 ? 1.0f : 0.0f, vertexMod == 2 ? 1.0f : 0.0f, );

    return output;
}

float4 GetTexture(uint32_t _textureID, float2 _uv)
{
    if (_textureID == TOMBSTONE_ID)
    {
        return float4(1.0);
    }

    return textures[NonUniformResourceIndex((uint)_textureID)].Sample(_uv);
}

[shader("fragment")]
FGOutput fragMain(VSOutput _vsOutput)
{
    const Material material = materials[_vsOutput.materialID];

    const float4 albedoTexture = GetTexture(material.albedoID, _vsOutput.uv0);

    if (albedoTexture.w < 0.1)
    {
        discard;
    }

    FGOutput output;

    output.pos = float4(_vsOutput.worldPos, 1.0);
    output.albedo = albedoTexture * float4(material.albedoColor, 1.0);

    float4 specularTexture = GetTexture(material.specularID, _vsOutput.uv0);

    if (material.mode == MaterialMode::SPECULAR)
    {
        output.specular = 
        { 
            specularTexture.x * material.specularColor.x, 
            specularTexture.y * material.specularColor.y,
            specularTexture.z * material.specularColor.z, 
            material.smoothness 
        };
    }
    else
    {
        output.specular = GetTexture(material.metallicID, _vsOutput.uv0);
    }

    output.normal = float4(_vsOutput.normal, (float)material.mode);

    if (material.normalID != TOMBSTONE_ID)
    {
        float4 normalTexture = GetTexture(material.normalID, _vsOutput.uv0);
        normalTexture = normalize(normalTexture * 2.0 - 1.0);

        output.normal.xyz = mul(normalTexture.xyz, _vsOutput.TBN);
    }

    if (material.emissiveID != TOMBSTONE_ID)
    {
        output.emissive = GetTexture(material.emissiveID, _vsOutput.uv0) * float4(material.emissiveColor, 1.0);
    }
    else
    {
        output.emissive = float4(0.0);
    }

    if (material.aoID != TOMBSTONE_ID)
    {
        output.ao = GetTexture(material.aoID, _vsOutput.uv0).xy;
        output.ao.y = 0.0;
    }
    else
    {
        output.ao = float2(0.0, 1.0);
    }

    return output;
}
