#define TOMBSTONE_ID 65535

#define NUM_FLOAT_IN_VERTEX_DATA 24
#define FLOAT_SIZE_IN_BYTES 4

#define LIGHTING_MODE_NORMALS 0
#define LIGHTING_MODE_WIREFRAME 1
#define LIGHTING_MODE_WIREFRAME_LIT 2
#define LIGHTING_MODE_UNLIT 3
#define LIGHTING_MODE_LIT 4

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

struct VSOutput
{
    float4 pos : SV_Position;

    float3 color;

    float3 normal;
    float3 tangent;
    float3 bitangent;

    float3 barycentric;

    float2 uv0;
    float2 uv1;

    nointerpolation uint materialID;
}

struct Matrices
{
    float4x4 mvp;
    float3x3 normal;
}

struct Material
{
    uint16_t albedoID;
    uint16_t specularID;
    uint16_t normalID;

    float3 albedoColor;
    float pad0;

    float3 specularColor;
    float shininess;
}

struct MetaData
{
    uint32_t indexOffset;
    uint32_t indexCount;
    uint32_t vertexOffset;

    uint16_t materialID;
}

struct DirLight
{
    float3 dir;
    float3 ambient;
    float3 diffuse;
    float3 specular;
}

struct PointLight
{
    float3 pos;
    float3 ambient;
    float3 diffuse;
    float3 specular;

    float constant;
    float linear;
    float quadratic;

    float radius;
}

struct SpotLight
{
    float3 pos;
    float3 dir;
    float3 ambient;
    float3 diffuse;
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

// Bindless data
[vk::binding(0, 0)]
Sampler2D textures[];

// Core data
[vk::binding(0, 1)]
StructuredBuffer<VertexData> vertices;

[vk::binding(1, 1)]
StructuredBuffer<uint32_t> indices;

[vk::binding(2, 1)]
StructuredBuffer<Material> materials;

// Scene data
[vk::binding(0, 2)]
StructuredBuffer<Matrices> matrices;

[vk::binding(1, 2)]
StructuredBuffer<MetaData> metaDatas;

// Lighting data
[vk::binding(0, 3)]
StructuredBuffer<DirLight> dirLights;

[vk::binding(1, 3)]
StructuredBuffer<PointLight> pointLights;

[vk::binding(2, 3)]
StructuredBuffer<SpotLight> spotLights;

[vk::binding(3,3)]
ConstantBuffer<LightData> lightData;

[vk::constant_id(0)]
const uint lightingMode = LIGHTING_MODE_NORMALS;

[vk::push_constant]
float3 camPos;

[shader("vertex")]
VSOutput vertMain(uint _vertexID : SV_VertexID, uint _instance: SV_StartInstanceLocation)
{
    const MetaData metaData = metaDatas[_instance];

    const Matrices matrix = matrices[_instance];

    const uint index = indices[metaData.indexOffset + _vertexID];

    const VertexData vertexData = vertices[metaData.vertexOffset + index];

    VSOutput output;

    output.pos = mul(float4(vertexData.pos, 1.0f), matrix.mvp);
    output.normal = mul(vertexData.normal, matrix.normal);

    output.tangent = vertexData.tangent;
    output.bitangent = vertexData.bitangent;

    output.color = vertexData.color;

    output.uv0 = vertexData.uv0;
    output.uv1 = vertexData.uv1;

    output.materialID = metaData.materialID;

    uint vertexMod = _vertexID % 3;
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
        return float4(1.0f);
    }

    return textures[NonUniformResourceIndex((uint)_textureID)].Sample(_uv);
}

float4 CalcDirLight(DirLight _light, VSOutput _vertIn)
{
    const Material material = materials[_vertIn.materialID];

    const float3 fragPos = float3(_vertIn.pos.x, _vertIn.pos.y, _vertIn.pos.z);

    const float4 albedoTex = GetTexture(material.albedoID, _vertIn.uv0) * float4(material.albedoColor, 1.0f);
    const float4 specularTex = GetTexture(material.specularID, _vertIn.uv0) * float4(material.specularColor, 1.0f);

    const float3 ambient = _light.ambient * float3(albedoTex.x, albedoTex.y, albedoTex.z);

    // Diffuse
    const float3 normal = normalize(camPos - fragPos);
    const float3 lightDir = normalize(-_light.dir);
    const float diffuse = max(dot(normal, lightDir), 0.0f);
    const float3 finalDiffuse = _light.diffuse * diffuse * float3(albedoTex.x, albedoTex.y, albedoTex.z);

    // Specular
    const float3 viewDir = normalize(camPos - fragPos);
    const float3 reflectionDir = reflect(-lightDir, normal);
    const float specular = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);
    const float3 finalSpecular = _light.specular * specular * float3(specularTex.x, specularTex.y, specularTex.z);

    return float4(ambient + diffuse + finalSpecular, 1.0f);
}

float4 CalcPointLight(PointLight _light, VSOutput _vertIn)
{
    const Material material = materials[_vertIn.materialID];

    const float3 fragPos = float3(_vertIn.pos.x, _vertIn.pos.y, _vertIn.pos.z);

    const float3 lightVec = _light.pos - fragPos;

    const float dist = length(lightVec);
    const float attenuation = 1.0 / (_light.constant + _light.linear * dist + _light.quadratic * (dist * dist));

    const float4 albedoTex = GetTexture(material.albedoID, _vertIn.uv0) * float4(material.albedoColor, 1.0f);
    const float4 specularTex = GetTexture(material.specularID, _vertIn.uv0) * float4(material.specularColor, 1.0f);

    float3 ambient = _light.ambient * float3(albedoTex.x, albedoTex.y, albedoTex.z);

    // Diffuse
    const float3 normal = normalize(_vertIn.normal);
    const float3 lightDir = normalize(lightVec);
    const float diffuse = max(dot(normal, lightDir), 0.0f);
    float3 finalDiffuse = _light.diffuse * diffuse * float3(albedoTex.x, albedoTex.y, albedoTex.z);

    // Specular
    const float3 viewDir = normalize(camPos - fragPos);
    const float3 reflectionDir = reflect(-lightDir, normal);
    const float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);
    float3 finalSpec = _light.specular * spec * float3(specularTex.x, specularTex.y, specularTex.z);

    ambient *= attenuation;
    finalDiffuse *= attenuation;
    finalSpec *= attenuation;

    return float4(ambient + finalDiffuse + finalSpec, 1.0f);
}

float4 CalcSpotLight(SpotLight _light, VSOutput _vertIn)
{
    const Material material = materials[_vertIn.materialID];

    const float3 fragPos = float3(_vertIn.pos.x, _vertIn.pos.y, _vertIn.pos.z);

    const float3 lightVec = _light.pos - fragPos;
    const float3 lightDir = normalize(lightVec);

    const float theta = dot(lightDir, normalize(-_light.dir));

    const float4 albedoTex = GetTexture(material.albedoID, _vertIn.uv0) * float4(material.albedoColor, 1.0f);

    if (theta > _light.cutOff)
    {
        const float epsilon = _light.cutOff - _light.outerCutOff;
        const float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0f, 1.0f);

        const float4 specularTex = GetTexture(material.specularID, _vertIn.uv0) * float4(material.specularColor, 1.0f);

        const float3 ambient = _light.ambient * float3(albedoTex.x, albedoTex.y, albedoTex.z);

        // Diffuse
        const float3 normal = normalize(_vertIn.normal);
        const float diffuse = max(dot(normal, lightDir), 0.0f);
        float3 finalDiffuse = _light.diffuse * diffuse * float3(albedoTex.x, albedoTex.y, albedoTex.z);

        // Specular
        const float3 viewDir = normalize(camPos - fragPos);
        const float3 reflectionDir = reflect(-_light.dir, normal);
        const float specular = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);
        float3 finalSpecular = _light.specular * specular * float3(specularTex.x, specularTex.y, specularTex.z);

        finalDiffuse *= intensity;
        finalSpecular *= intensity;

        return float4(ambient + finalDiffuse + finalSpecular, 1.0f);
    }
    else
    {
        return float4(_light.ambient * float3(albedoTex.x, albedoTex.y, albedoTex.z), 1.0f);
    }
}

float4 CalcWireframe(float3 _bary, float4 _color, float4 _wireframeColor)
{
    const float minBary = min(min(_bary.x, _bary.y), _bary.z);
    const float edgeDelta = fwidth(minBary);
    const float thickness = 1.5f * edgeDelta;
    const float edgeFactor = smoothstep(thickness - edgeDelta, thickness + edgeDelta, minBary);

    return lerp(_color, _wireframeColor, 1.0f);
}

[shader("fragment")]
float4 fragMain(VSOutput _vertIn) : SV_Target
{
    float4 color = float4(0.0f);

    switch (lightingMode)
    {
    case LIGHTING_MODE_NORMALS:
        color = float4(_vertIn.normal, 1.0f);
        break;
    case LIGHTING_MODE_WIREFRAME:
        color = CalcWireframe(_vertIn.barycentric, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f));
        break;
    case LIGHTING_MODE_WIREFRAME_LIT:
        for (uint i = 0; i < lightData.nbDirLight; i++)
        {
            color += CalcDirLight(dirLights[i], _vertIn);
        }

        for (uint i = 0; i < lightData.nbPointLight; i++)
        {
            color += CalcPointLight(pointLights[i], _vertIn);
        }

        for (uint i = 0; i < lightData.nbSpotLight; i++)
        {
            color += CalcSpotLight(spotLights[i], _vertIn);
        }

        color = color = CalcWireframe(_vertIn.barycentric, color, float4(0.0f, 0.0f, 0.0f, 1.0f));
        break;
    case LIGHTING_MODE_UNLIT:
        color = GetTexture(materials[_vertIn.materialID].albedoID, _vertIn.uv0) * float4(materials[_vertIn.materialID].albedoColor, 1.0f);
        break;
    case LIGHTING_MODE_LIT:
        for (uint i = 0; i < lightData.nbDirLight; i++)
        {
            color += CalcDirLight(dirLights[i], _vertIn);
        }

        for (uint i = 0; i < lightData.nbPointLight; i++)
        {
            color += CalcPointLight(pointLights[i], _vertIn);
        }

        for (uint i = 0; i < lightData.nbSpotLight; i++)
        {
            color += CalcSpotLight(spotLights[i], _vertIn);
        }
        break;
    }

    return color;
}