#include "PointLight.h"

#include <iostream>

#include "Transform.h"
#include "../LightManager/LightManager.h"
#include "../LightManager/PointLightData.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	PointLight::PointLight()
	{
		ALLOCATE_MEMORY(m_data);
		LightManager::Get()->AddPointLight(m_data, &m_lightId);
	}

	PointLight::~PointLight()
	{

	}

	void PointLight::Draw(Camera* _camera, Transform* _transform)
	{
		SetPosition(_transform->position);
	}

	glm::vec3 PointLight::GetPosition() const
	{
		return m_data->position;
	}

	void PointLight::SetPosition(glm::vec3 _newPosition) const
	{
		m_data->position = _newPosition;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}

	glm::vec3 PointLight::GetAmbient() const
	{
		return m_data->ambient;
	}

	void PointLight::SetAmbient(glm::vec3 _newAmbient) const
	{
		m_data->ambient = _newAmbient;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}

	glm::vec3 PointLight::GetDiffuse() const
	{
		return m_data->diffuse;
	}

	void PointLight::SetDiffuse(glm::vec3 _newDiffuse) const
	{
		m_data->diffuse = _newDiffuse;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}

	glm::vec3 PointLight::GetSpecular() const
	{
		return m_data->specular;
	}

	void PointLight::SetSpecular(glm::vec3 _newSpecular) const
	{
		m_data->specular = _newSpecular;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}

	float PointLight::GetConstant() const
	{
		return m_data->constant;
	}

	void PointLight::SetConstant(float _newConstant) const
	{
		m_data->constant = _newConstant;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}

	float PointLight::GetLinear() const
	{
		return m_data->linear;
	}

	void PointLight::SetLinear(float _newLinear) const
	{
		m_data->linear = _newLinear;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}

	float PointLight::GetQuadratic() const
	{
		return m_data->quadratic;
	}

	void PointLight::SetQuadratic(float _newQuadratic) const
	{
		m_data->quadratic = _newQuadratic;
		LightManager::Get()->UpdatePointLight(m_data, m_lightId);
	}
}
