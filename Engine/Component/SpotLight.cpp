#include "SpotLight.h"

#include "Transform.h"
#include "../LightManager/LightManager.h"
#include "../LightManager/SpotLightData.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	SpotLight::SpotLight()
	{
		ALLOCATE_MEMORY(m_data);
		LightManager::Get()->AddSpotLight(m_data, &m_lightId);
	}

	SpotLight::~SpotLight()
	{
	}

	void SpotLight::Draw(Camera* _camera, Transform* _transform)
	{
		SetPosition(_transform->position);
	}

	glm::vec3 SpotLight::GetPosition()
	{
		return m_data->position;
	}

	void SpotLight::SetPosition(glm::vec3 _newPosition)
	{
		m_data->position = _newPosition;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}

	glm::vec3 SpotLight::GetDirection()
	{
		return m_data->direction;
	}

	void SpotLight::SetDirection(glm::vec3 _newDirection)
	{
		m_data->direction = _newDirection;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}

	glm::vec3 SpotLight::GetAmbient()
	{
		return m_data->ambient;
	}

	void SpotLight::SetAmbient(glm::vec3 _newAmbient)
	{
		m_data->ambient = _newAmbient;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}

	glm::vec3 SpotLight::GetDiffuse()
	{
		return m_data->diffuse;
	}

	void SpotLight::SetDiffuse(glm::vec3 _newDiffuse)
	{
		m_data->diffuse = _newDiffuse;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}

	glm::vec3 SpotLight::GetSpecular()
	{
		return m_data->specular;
	}

	void SpotLight::SetSpecular(glm::vec3 _newSpecular)
	{
		m_data->specular = _newSpecular;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}

	float SpotLight::GetCutOff()
	{
		return m_data->cutOff;
	}

	void SpotLight::SetCutOff(float _newCutOff)
	{
		m_data->cutOff = _newCutOff;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}

	float SpotLight::GetOuterCutOff()
	{
		return m_data->outerCutOff;
	}

	void SpotLight::SetOuterCutOff(float _newOuterCutOff)
	{
		m_data->outerCutOff = _newOuterCutOff;
		LightManager::Get()->UpdateSpotLight(m_data, m_lightId);
	}
}
