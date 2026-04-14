#include "DirectLight.h"

#include "../LightManager/DirectLightData.h"

namespace Llyn
{
	DirectLight::DirectLight()
	{

	}

	DirectLight::~DirectLight()
	{

	}

	void DirectLight::Draw(Camera* _camera, Transform* _transform)
	{

	}

	glm::vec3 DirectLight::GetDir()
	{
		return m_data->dir;
	}

	void DirectLight::SetDir(glm::vec3 _newDir)
	{
		m_data->dir = _newDir;
	}

	glm::vec3 DirectLight::GetAmbient()
	{
		return m_data->ambient;
	}

	void DirectLight::SetAmbient(glm::vec3 _newAmbient)
	{
		m_data->ambient = _newAmbient;
	}

	glm::vec3 DirectLight::GetDiffuse()
	{
		return m_data->diffuse;
	}

	void DirectLight::SetDiffuse(glm::vec3 _newDiffuse)
	{
		m_data->diffuse = _newDiffuse;
	}

	glm::vec3 DirectLight::GetSpecular()
	{
		return m_data->specular;
	}

	void DirectLight::SetSpecular(glm::vec3 _newSpecular)
	{
		m_data->specular = _newSpecular;
	}
}
