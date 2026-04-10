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

struct Light 
{
	int type;

	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
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
uniform Light light;


vec4 PointLight()
{	
	vec3 lightVec = light.position - fsIn.FragPos;

	float dist = length(lightVec);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	vec3 diffuseTex = texture(material.baseMap, fsIn.TexCoord).rgb * material.baseColor;
	vec3 specularTex = texture(material.specularMap, fsIn.TexCoord).rgb * material.specularColor;

	// ambient
	vec3 ambient = light.ambient * diffuseTex;

	// diffuse
	vec3 normal = normalize(fsIn.Normal);
	vec3 lightDir = normalize(lightVec);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseTex;

	// specular 
	vec3 viewDir = normalize(position - fsIn.FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);

	vec3 specular = light.specular * spec * specularTex;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec4(ambient + diffuse + specular, 1.0);
}

vec4 DirectLight()
{
	vec3 diffuseTex = texture(material.baseMap, fsIn.TexCoord).rgb * material.baseColor;
	vec3 specularTex = texture(material.specularMap, fsIn.TexCoord).rgb * material.specularColor;

	// ambient
	vec3 ambient = light.ambient * diffuseTex;

	// diffuse
	vec3 normal = normalize(fsIn.Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseTex;

	// specular 
	vec3 viewDir = normalize(position - fsIn.FragPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);

	vec3 specular = light.specular * spec * specularTex;

	return vec4(ambient + diffuse + specular, 1.0);
}

vec4 SpotLight()
{
	vec3 lightVec = light.position - fsIn.FragPos;

	vec3 diffuseTex = texture(material.baseMap, fsIn.TexCoord).rgb * material.baseColor;

	vec3 lightDir = normalize(lightVec);

	float theta = dot(lightDir, normalize(-light.direction));

	vec4 color = vec4(1.0);

	if (theta > light.cutOff)
	{
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 specularTex = texture(material.specularMap, fsIn.TexCoord).rgb * material.specularColor;

		// ambient
		vec3 ambient = light.ambient * diffuseTex;

		// diffuse
		vec3 normal = normalize(fsIn.Normal);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * diffuseTex;

		// specular 
		vec3 viewDir = normalize(position - fsIn.FragPos);
		vec3 reflectionDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectionDir), 0.0f), material.shininess);

		vec3 specular = light.specular * spec * specularTex;

		diffuse *= intensity;
		specular *= intensity;

		color = vec4(ambient + diffuse + specular, 1.0);
	}	
	else 
	{
		color = vec4(light.ambient * diffuseTex, 1.0);
	}

	return color;
}


void main()
{	
	if (texture(material.baseMap, fsIn.TexCoord).a < 0.1)
	{
		discard;
	}

	vec4 finalColor = vec4(1.0);

	if (light.type == 0)
	{
		finalColor = PointLight();
	}
	else if (light.type == 1)
	{
		finalColor = DirectLight();
	}
	else 
	{
		finalColor = SpotLight();
	}

    FragColor = vec4(finalColor);
}