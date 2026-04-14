#version 460 core

out vec4 FragColor;

in VS_OUT 
{
	vec3 FragPos;
	vec3 Normal;
	vec3 Color;
	vec2 TexCoord;
} fsIn;

struct Material
{
	sampler2D baseMap;
	sampler2D specularMap;
	sampler2D normalMap;

	vec3 baseColor;
	vec3 specularColor;

	float shininess;
};

struct DirLight
{
	vec3 dir;
	float padding1;

	vec3 ambient;
	float padding2;

	vec3 diffuse;
	float padding3;

	vec3 specular;
	float padding4;
};

struct PointLight
{
	vec3 pos;
	float constant;

	vec3 ambient;
	float linear;

	vec3 diffuse;
	float quadratic;

	vec3 specular;
	float padding1;
};

struct SpotLight
{
	vec3 pos;
	float cutOff;

	vec3 dir;
	float outerCutOff;

	vec3 ambient;
	float padding1;

	vec3 diffuse;
	float padding2;

	vec3 specular;
	float padding3;
};

layout(std140, binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;

	vec3 position;
	float padding;
};


layout(std430, binding = 0) buffer DirectionalLights
{
	int nbDirectLights;
	int padding1[3];
	DirLight dirLights[];
};

layout(std430, binding = 1) buffer PointLights
{
	int nbPointLights;
	int padding2[3];
	PointLight pointLights[];
};

layout(std430, binding = 2) buffer SpotLights
{
	int nbSpotLights;
	int padding3[3];
	SpotLight spotLights[];
};

uniform Material material;

vec4 CalcPointLight(PointLight _light)
{	
	vec3 lightVec = _light.pos - fsIn.FragPos;

	float dist = length(lightVec);
	float attenuation = 1.0 / (_light.constant + _light.linear * dist + _light.quadratic * (dist * dist));

	vec3 diffuseTex = texture(material.baseMap, fsIn.TexCoord).rgb * material.baseColor;
	vec3 specularTex = texture(material.specularMap, fsIn.TexCoord).rgb * material.specularColor;

	// ambient
	vec3 ambient = _light.ambient * diffuseTex;

	// diffuse
	vec3 normal = normalize(fsIn.Normal);
	vec3 lightDir = normalize(lightVec);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = _light.diffuse * diff * diffuseTex;

	// specular 
	vec3 viewDir = normalize(position - fsIn.FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);

	vec3 specular = _light.specular * spec * specularTex;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4(ambient + diffuse + specular, 1.0);
}

vec4 CalcDirectLight(DirLight _light)
{
	vec3 diffuseTex = texture(material.baseMap, fsIn.TexCoord).rgb * material.baseColor;
	vec3 specularTex = texture(material.specularMap, fsIn.TexCoord).rgb * material.specularColor;

	// ambient
	vec3 ambient = _light.ambient * diffuseTex;

	// diffuse
	vec3 normal = normalize(fsIn.Normal);
	vec3 lightDir = normalize(-_light.dir);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = _light.diffuse * diff * diffuseTex;

	// specular 
	vec3 viewDir = normalize(position - fsIn.FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);

	vec3 specular = _light.specular * spec * specularTex;

	return vec4(ambient + diffuse + specular, 1.0);
}

vec4 CalcSpotLight(SpotLight _light)
{
	vec3 lightVec = _light.pos - fsIn.FragPos;

	vec3 diffuseTex = texture(material.baseMap, fsIn.TexCoord).rgb * material.baseColor;

	vec3 lightDir = normalize(lightVec);

	float theta = dot(lightDir, normalize(-_light.dir));

	vec4 color = vec4(1.0);

	if (theta > _light.cutOff)
	{
		float epsilon = _light.cutOff - _light.outerCutOff;
		float intensity = clamp((theta - _light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 specularTex = texture(material.specularMap, fsIn.TexCoord).rgb * material.specularColor;

		// ambient
		vec3 ambient = _light.ambient * diffuseTex;

		// diffuse
		vec3 normal = normalize(fsIn.Normal);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = _light.diffuse * diff * diffuseTex;

		// specular 
		vec3 viewDir = normalize(position - fsIn.FragPos);
		vec3 reflectionDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);

		vec3 specular = _light.specular * spec * specularTex;

		diffuse *= intensity;
		specular *= intensity;

		color = vec4(ambient + diffuse + specular, 1.0);
	}	
	else 
	{
		color = vec4(_light.ambient * diffuseTex, 1.0);
	}

	return color;
}


void main()
{	
	if (texture(material.baseMap, fsIn.TexCoord).a < 0.1)
	{
		discard;
	}

	vec4 finalColor = vec4(0.0f);

	for (int i = 0; i < nbDirectLights; ++i)
	{
		finalColor += CalcDirectLight(dirLights[i]);
	}
	for (int i = 0; i < nbPointLights; ++i)
	{
		finalColor += CalcPointLight(pointLights[i]);
	}
	for (int i = 0; i < nbSpotLights; ++i)
	{
		finalColor += CalcSpotLight(spotLights[i]);
	}

    FragColor = vec4(finalColor);
}