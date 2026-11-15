#define PI 3.14159265359

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
Texture2D<float4> gPosition;

[vk::binding(1, 0)]
Texture2D<float4> gNormal;

[vk::binding(2, 0)]
Texture2D<float4> gAlbedo;

[vk::binding(3, 0)]
Texture2D<float4> gSpecular;

[vk::binding(4, 0)]
Texture2D<float4> gEmissive;

[vk::binding(5, 0)]
Texture2D<float2> gAO;

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

float3 CalcDirLight(DirLight _light, float4 _pos, float4 _albedo, float4 _specular, float4 _normal)
{
    const float3 ambient = _light.ambient * _albedo.xyz;

    // Diffuse
    const float3 normal = normalize(_normal.xyz);
    const float3 lightDir = normalize(-_light.dir);
    const float diffuse = max(dot(normal, lightDir), 0.0);
    const float3 finalDiffuse = _light.diffuse * diffuse * float3(_albedo.x, _albedo.y, _albedo.z);

    // Specular
    const float3 viewDir = normalize(camPos - _pos.xyz);
    const float3 halfWayDir = normalize(lightDir + viewDir);
    const float specular = pow(max(dot(normal, halfWayDir), 0.0), SmoothnessToShininess(_specular.w));
    const float3 finalSpecular = _light.specular * specular * _specular.xyz;

    return ambient + finalDiffuse + finalSpecular;
}

float3 CalcPointLight(PointLight _light, float4 _pos, float4 _albedo, float4 _specular, float4 _normal)
{
    const float3 lightVec = _light.pos - _pos.xyz;
    const float distance = length(lightVec);

    const float attenuation = 1.0 / (_light.constant + _light.linear * distance + _light.quadratic * (distance * distance));

    const float3 ambient = _light.ambient * _albedo.xyz * attenuation;

    // Diffuse
    const float3 normal = normalize(_normal.xyz);
    const float3 lightDir = normalize(lightVec);
    const float diffuse = max(dot(normal, lightDir), 0.0);
    const float3 finalDiffuse = (_light.diffuse * diffuse * _albedo.xyz) * attenuation;

    // Specular
    const float3 viewDir = normalize(camPos - _pos.xyz);
    const float3 halfwayDir = normalize(lightDir + viewDir);
    const float specular = pow(max(dot(normal, halfwayDir), 0.0), SmoothnessToShininess(_specular.w));
    const float3 finalSpecular = _light.specular * specular * _specular.xyz * attenuation;

    return ambient + finalDiffuse + finalSpecular;
}

float3 CalcSpotLight(SpotLight _light, float4 _pos, float4 _albedo, float4 _specular, float4 _normal)
{
    const float3 lightVec = _light.pos - _pos.xyz;
    const float3 lightDir = normalize(lightVec);

    const float theta = dot(lightDir, normalize(-_light.dir));

    if (theta > _light.cutOff)
    {
        const float epsilon = _light.cutOff - _light.outerCutOff;
        const float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0, 1.0);

        const float3 ambient = _light.ambient * _albedo.xyz;

        // Diffuse
        const float3 normal = normalize(_normal.xyz);
        const float diffuse = max(dot(normal, lightDir), 0.0);
        const float3 finalDiffuse = _light.diffuse * diffuse * float3(_albedo.x, _albedo.y, _albedo.z) * intensity;

        // Specular
        const float3 viewDir = normalize(_normal.xyz);
        const float3 halfwayDir = normalize(lightDir + viewDir);
        const float specular = pow(max(dot(normal, halfwayDir), 0.0), SmoothnessToShininess(_specular.w));
        const float3 finalSpecular = _light.specular * specular * _specular.xyz * intensity;

        return ambient + finalDiffuse + finalSpecular;
    }
    else
    {
        return _light.ambient * _albedo.xyz;
    }
}

float DistributionGGX(float3 _n, float3 _h, float _roughness)
{
    const float a = _roughness * _roughness;
    const float a2 = a * a;
    const float nDotH = max(dot(_n, _h), 0.0);
    const float nDotH2 = nDotH * nDotH;

    const float num = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float _nDotV, float _roughness)
{
    const float r = _roughness + 1.0;
    const float k = (r * r) / 8.0;

    const float num = _nDotV;
    const float denom = _nDotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(float3 _n, float3 _v, float3 _l, float _roughness)
{
    const float nDotV = max(dot(_n, _v), 0.0);
    const float nDotL = max(dot(_n, _l), 0.0);
    const float ggx2 = GeometrySchlickGGX(nDotV, _roughness);
    const float ggx1 = GeometrySchlickGGX(nDotL, _roughness);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float _cosTheta, float3 _f0)
{
    return _f0 + (1.0 - _f0) * pow(clamp(1.0 - _cosTheta, 0.0, 1.0), 5.0);
}

float3 CalcPBRDirLight(DirLight _light, float4 _pos, float4 _albedo, float4 _metallic, float4 _normal)
{
    const float metallic = _metallic.z;
    const float roughness = 1.0 - (1.0 - _metallic.y) * _metallic.w;

    const float3 viewDir = normalize(camPos - _pos.xyz);

    float3 f0 = 0.04;
    f0 = lerp(f0, _albedo.xyz, metallic);

    const float3 normal = normalize(_normal.xyz);
    const float3 lightDir = normalize(-_light.dir);
    const float3 halfWayDir = normalize(lightDir + viewDir);

    float3 radiance = _light.diffuse;

    const float ndf = DistributionGGX(normal, halfWayDir, roughness);
    const float g = GeometrySmith(normal, viewDir, lightDir, roughness);
    const float3 f = FresnelSchlick(max(dot(halfWayDir, viewDir), 0.0), f0);

    const float3 kS = f;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - metallic;

    const float3 numerator = ndf * g * f;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    const float3 specular = numerator / denominator;

    const float nDotL = max(dot(normal, lightDir), 0.0);

    return (kD * _albedo.xyz / PI + specular) * radiance * nDotL;
}

float3 CalcPBRPointLight(PointLight _light, float4 _pos, float4 _albedo, float4 _metallic, float4 _normal)
{
    const float metallic = _metallic.z;
    const float roughness = 1.0 - (1.0 - _metallic.y) * _metallic.w;

    const float3 lightVec = _light.pos - _pos.xyz;
    const float3 viewDir = normalize(camPos - _pos.xyz);

    float3 f0 = 0.04;
    f0 = lerp(f0, _albedo.xyz, metallic);

    const float3 normal = normalize(_normal.xyz);
    const float3 lightDir = normalize(_light.pos - _pos.xyz);
    const float3 halfWayDir = normalize(lightDir + viewDir);

    const float distance = length(lightVec);
    const float attenuation = 1.0 / (_light.constant + _light.linear * distance + _light.quadratic * (distance * distance));

    float3 radiance = _light.diffuse * attenuation;

    const float ndf = DistributionGGX(normal, halfWayDir, roughness);
    const float g = GeometrySmith(normal, viewDir, lightDir, roughness);
    const float3 f = FresnelSchlick(max(dot(halfWayDir, viewDir), 0.0), f0);

    const float3 kS = f;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - metallic;

    const float3 numerator = ndf * g * f;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
    const float3 specular = numerator / denominator;

    const float nDotL = max(dot(normal, lightDir), 0.0);

    return (kD * _albedo.xyz / PI + specular) * radiance * nDotL;
}

float3 CalcPBRSpotLight(SpotLight _light, float4 _pos, float4 _albedo, float4 _metallic, float4 _normal)
{
    const float3 lightDir = normalize(_light.pos - _pos.xyz);

    const float theta = dot(lightDir, normalize(-_light.dir));

    if (theta > _light.cutOff)
    {
        const float metallic = _metallic.z;
        const float roughness = 1.0 - (1.0 - _metallic.y) * _metallic.w;

        const float3 viewDir = normalize(camPos - _pos.xyz);

        float3 f0 = 0.04;
        f0 = lerp(f0, _albedo.xyz, metallic);

        const float3 normal = normalize(_normal.xyz);
        const float3 halfWayDir = normalize(lightDir + viewDir);

        const float epsilon = _light.cutOff - _light.outerCutOff;
        const float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0, 1.0);
        float3 radiance = _light.diffuse * intensity;

        const float ndf = DistributionGGX(normal, halfWayDir, roughness);
        const float g = GeometrySmith(normal, viewDir, lightDir, roughness);
        const float3 f = FresnelSchlick(max(dot(halfWayDir, viewDir), 0.0), f0);

        const float3 kS = f;
        float3 kD = float3(1.0) - kS;
        kD *= 1.0 - metallic;

        const float3 numerator = ndf * g * f;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
        const float3 specular = numerator / denominator;

        const float nDotL = max(dot(normal, lightDir), 0.0);

        return (kD * _albedo.xyz / PI + specular) * radiance * nDotL;
    }
    else
    {
        return float3(0.0);
    }
}

[shader("fragment")]
float4 fragMain(float4 _fragPos : SV_Position) : SV_Target
{
    const float4 posTexture = gPosition.Load(int3(int2(_fragPos.xy), 0));
    const float4 normalTexture = gNormal.Load(int3(int2(_fragPos.xy), 0));
    const float4 albedoTexture = gAlbedo.Load(int3(int2(_fragPos.xy), 0));
    const float4 specularTexture = gSpecular.Load(int3(int2(_fragPos.xy), 0));
    const float4 emissiveTexture = gEmissive.Load(int3(int2(_fragPos.xy), 0));
    const float2 aoTexture = gAO.Load(int3(int2(_fragPos.xy), 0));

    float3 finalColor = float3(0.0);

    // Without PBR
    if (normalTexture.w == 0.0)
    {
        for (uint i = 0; i < lightData.nbDirLight; i++)
        {
            finalColor += CalcDirLight(dirLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
        }

        for (uint i = 0; i < lightData.nbPointLight; i++)
        {
            const float distance = length(pointLights[i].pos - posTexture.xyz);
            if (distance < pointLights[i].radius)
            {
                finalColor += CalcPointLight(pointLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
            }
        }

        for (uint i = 0; i < lightData.nbSpotLight; i++)
        {
            finalColor += CalcSpotLight(spotLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
        }

        float ao = 1.0;
        if (aoTexture.y == 0.0)
        {
            ao = aoTexture.x;
        }

        finalColor *= ao;
    }
    else // With PBR
    {
        float3 ambientAccum = float3(0.0);
        for (uint i = 0; i < lightData.nbDirLight; i++)
        {
            finalColor += CalcPBRDirLight(dirLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
            ambientAccum += dirLights[i].ambient;
        }

        for(uint i = 0; i < lightData.nbPointLight; i++)
        {
            const float distance = length(pointLights[i].pos - posTexture.xyz);
            if (distance < pointLights[i].radius)
            {
                finalColor += CalcPBRPointLight(pointLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
                ambientAccum += pointLights[i].ambient;
            }
        }

        for (uint i = 0; i < lightData.nbSpotLight; i++)
        {
            finalColor += CalcPBRSpotLight(spotLights[i], posTexture, albedoTexture, specularTexture, normalTexture);
            ambientAccum += spotLights[i].ambient;
        }

        float ao = 1.0;
        if (aoTexture.y == 0.0)
        {
            ao = aoTexture.x;
        }

        finalColor += ambientAccum * albedoTexture.xyz * ao;
    }

    if (emissiveTexture.w > 0.0)
    {
        finalColor += emissiveTexture.xyz;
    }

    return float4(finalColor, 1.0);
}