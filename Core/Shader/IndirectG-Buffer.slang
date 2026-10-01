#define TOMBSTONE_ID 65535

struct VertexData
{
    float3 pos;
    float pad0;

    float3 normal;
    float pad1;

    float3 tangent;
    float pad2;

    float3 bitangent;
    float pad3;

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

struct Material
{
    uint16_t albedoID;
    uint16_t specularID;
    uint16_t normalID;

    float3 albedoColor;
    float pad0;

    float3 specularColor;
    float smoothness;
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
    float3 tangent;
    float3 bitangent;

    float3 barycentric;

    float2 uv0;
    float2 uv1;

    nointerpolation uint materialID;
}

struct FGOutput
{
    float3 pos : SV_Target0;
    float3 normal : SV_Target1;
    float3 albedo : SV_Target2;
    float2 specular : SV_Target3;
}

// Bindless
[vk::binding(0, 0)]
Sampler2D textures[];

// Core
[vk::binding(0, 1)]
StructuredBuffer<VertexData> verticies;

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

    const VertexData vertexData = verticies[metaData.vertexOffset + index];

    VSOutput output;
    output.pos = mul(float4(vertexData.pos, 1.0), matrix.mvp);
    output.worldPos = mul(vertexData.pos, float3x3(matrix.localPos));
    output.normal = mul(vertexData.pos, float3x3(matrix.normal));

    output.tangent = vertexData.tangent;
    output.bitangent = vertexData.bitangent;

    output.color = vertexData.color;

    output.uv0 = vertexData.uv0;
    output.uv1 = vertexData.uv1;

    output.materialID = metaData.materialID;

    const uint vertexMod = _vertexID % 3;
    output.barycentric = float3(
        vertexMod == 0 ? 1.0f : 0.0f,
        vertexMod == 1 ? 1.0f : 0.0f,
        vertexMod == 2 ? 1.0f : 0.0f,
    );

    return output;
}

float4 GetTexture(uint32_t _textureID, float2 _uv)
{
    if (_textureID == TOMBSTONE_ID)
    {
        return float4(0.0);
    }

    return textures[NonUniformResourceIndex((uint)_textureID)].Sample(_uv);
}

[shader("fragment")]
FGOutput fragMain(VSOutput _vsOutput)
{
    const Material material = materials[_vsOutput.materialID];

    FGOutput output;

    output.pos = _vsOutput.worldPos;
    output.normal = _vsOutput.normal;
    output.albedo = GetTexture(material.albedoID, _vsOutput.uv0).xyz;
    output.specular = { GetTexture(material.specularID, _vsOutput.uv0).x, material.smoothness };

    return output;
}