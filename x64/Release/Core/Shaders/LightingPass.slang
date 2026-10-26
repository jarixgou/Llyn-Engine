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

// G-Buffer texture pass
[vk::binding(0, 0)]
Texture2D<float3> gPosition;

[vk::binding(1, 0)]
Texture2D<float3> gNormal;

[vk::binding(2, 0)]
Texture2D<float4> gAlbedo;

[vk::binding(3, 0)]
Texture2D<float4> gSpecular;

// Ligthing data
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

static const float2 vertices[6] =
{
    { -1.0, -1.0 }, { 1.0, -1.0 }, { 1.0, 1.0 },
    { -1.0, -1.0 }, { 1.0, 1.0 },  { -1.0, 1.0 }
};

[shader("vertex")]
float4 vertMain(uint _vertexID: SV_VertexID) : SV_Position
{
    float2 vertex = vertices[_vertexID];

    return float4(vertex, 0, 1.0);
}

float SmoothnessToShininess(float _smoothness)
{
    return exp2(1.0 + _smoothness * 10.0);
}

float3 CalcDirLight(DirLight _light, float3 _pos, float4 _albedo, float4 _specular, float3 _normal)
{
    const float3 ambient = _light.ambient * float3(_albedo.x, _albedo.y, _albedo.z);

    // Diffuse
    const float3 normal = normalize(_normal);
    const float3 lightDir = normalize(-_light.dir);
    const float diffuse = max(dot(normal, lightDir), 0.0);
    const float3 finalDiffuse = _light.diffuse * diffuse * float3(_albedo.x, _albedo.y, _albedo.z);

    // Specular
    const float3 viewDir = normalize(camPos - _pos);
    const float3 halfWayDir = normalize(lightDir + viewDir);
    const float specular = pow(max(dot(normal, halfWayDir), 0.0), SmoothnessToShininess(_specular.w));
    const float3 finalSpecular = _light.specular * specular * float3(_specular.x, _specular.y, _specular.z);

    return ambient + finalDiffuse + finalSpecular;
}

float3 CalcPointLight(PointLight _light, float3 _pos, float4 _albedo, float4 _specular, float3 _normal)
{
    const float3 lightVec = _light.pos - _pos;
    const float distance = length(lightVec);

    const float attenuation = 1.0 / (_light.constant + _light.linear * distance + _light.quadratic * (distance * distance));

    const float3 ambient = _light.ambient * float3(_albedo.x, _albedo.y, _albedo.z) * attenuation;

    // Diffuse
    const float3 normal = normalize(_normal);
    const float3 lightDir = normalize(lightVec);
    const float diffuse = max(dot(normal, lightDir), 0.0);
    const float3 finalDiffuse = (_light.diffuse * diffuse * float3(_albedo.x, _albedo.y, _albedo.z)) * attenuation;

    // Specular
    const float3 viewDir = normalize(camPos - _pos);
    const float3 halfwayDir = normalize(lightDir + viewDir);
    const float specular = pow(max(dot(normal, halfwayDir), 0.0), SmoothnessToShininess(_specular.w));
    const float3 finalSpecular = (_light.specular * specular * float3(_specular.x, _specular.y, _specular.z)) * attenuation;

    return ambient + finalDiffuse + finalSpecular;
}

float3 CalcSpotLight(SpotLight _light, float3 _pos, float4 _albedo, float4 _specular, float3 _normal)
{
    const float3 lightVec = _light.pos - _pos;
    const float3 lightDir = normalize(lightVec);

    const float theta = dot(lightDir, normalize(-_light.dir));

    if (theta > _light.cutOff)
    {
        const float epsilon = _light.cutOff - _light.outerCutOff;
        const float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0, 1.0);

        const float3 ambient = _light.ambient * float3(_albedo.x, _albedo.y, _albedo.z);

        // Diffuse
        const float3 normal = normalize(_normal);
        const float diffuse = max(dot(normal, lightDir), 0.0);
        const float3 finalDiffuse = _light.diffuse * diffuse * float3(_albedo.x, _albedo.y, _albedo.z) * intensity;

        // Specular
        const float3 viewDir = normalize(_normal);
        const float3 halfwayDir = normalize(lightDir + viewDir);
        const float specular = pow(max(dot(normal, halfwayDir), 0.0), SmoothnessToShininess(_specular.w));
        const float3 finalSpecular = _light.specular * specular * float3(_specular.x, _specular.y, _specular.z) * intensity;

        return ambient + finalDiffuse + finalSpecular;
    }
    else
    {
        return _light.ambient * float3(_albedo.x, _albedo.y, _albedo.z);
    }
}

[shader("fragment")]
float4 fragMain(float4 _fragPos : SV_Position) : SV_Target
{
    const float3 posTexture = gPosition.Load(int3(int2(_fragPos.xy), 0));
    const float3 normalTexture = gNormal.Load(int3(int2(_fragPos.xy), 0));
    const float4 albedoTexture = gAlbedo.Load(int3(int2(_fragPos.xy), 0));
    const float4 specularTexture = gSpecular.Load(int3(int2(_fragPos.xy), 0));

    float3 finalColor = float3(0.0);

    for (uint i = 0; i < lightData.nbDirLight; i++)
    {
        finalColor += CalcDirLight(dirLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
    }

    for (uint i = 0; i < lightData.nbPointLight; i++)
    {
        const float distance = length(pointLights[i].pos - posTexture);
        if (distance < pointLights[i].radius)
        {
            finalColor += CalcPointLight(pointLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
        }
    }

    for (uint i = 0; i < lightData.nbSpotLight; i++)
    {
        finalColor += CalcSpotLight(spotLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
    }

    return float4(finalColor, 1.0);
}