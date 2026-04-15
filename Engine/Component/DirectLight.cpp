#include "DirectLight.h"

#include "../LightManager/DirectLightData.h"
#include "../LightManager/LightManager.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	DirectLight::DirectLight()
	{
		ALLOCATE_MEMORY(m_data);
		LightManager::Get()->AddDirectLight(m_data, &m_lightId);
	}

	DirectLight::~DirectLight()
	{

	}

	void DirectLight::Draw(Camera* _camera, Transform* _transform)
	{

	}

	glm::vec3 DirectLight::GetDirection()
	{
		return m_data->dir;
	}

	void DirectLight::SetDir(glm::vec3 _newDir)
	{
		m_data->dir = _newDir;
		LightManager::Get()->UpdateDirectLight(m_data, m_lightId);
	}

	glm::vec3 DirectLight::GetAmbient()
	{
		return m_data->ambient;
	}

	void DirectLight::SetAmbient(glm::vec3 _newAmbient)
	{
		m_data->ambient = _newAmbient;
		LightManager::Get()->UpdateDirectLight(m_data, m_lightId);
	}

	glm::vec3 DirectLight::GetDiffuse()
	{
		return m_data->diffuse;
	}

	void DirectLight::SetDiffuse(glm::vec3 _newDiffuse)
	{
		m_data->diffuse = _newDiffuse;
		LightManager::Get()->UpdateDirectLight(m_data, m_lightId);
	}

	glm::vec3 DirectLight::GetSpecular()
	{
		return m_data->specular;
	}

	void DirectLight::SetSpecular(glm::vec3 _newSpecular)
	{
		m_data->specular = _newSpecular;
		LightManager::Get()->UpdateDirectLight(m_data, m_lightId);
	}
}
