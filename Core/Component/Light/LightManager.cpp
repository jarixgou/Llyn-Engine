#include "LightManager.h"

#include "../../Entities/ECS.h"
#include "../../Render/UniformManager.h"

void LightManager::Init()
{
	m_init = false;

	m_dirLightsCache.clear();
	m_pointLightsCache.clear();
	m_spotLightsCache.clear();

	m_frameSendsDirLight.resize(MAX_FRAMES_IN_FLIGHT, false);
	m_frameSendsPointLight.resize(MAX_FRAMES_IN_FLIGHT, false);
	m_frameSendsSpotLight.resize(MAX_FRAMES_IN_FLIGHT, false);
}

void LightManager::UpdateLights(ECS* _ecs, IUniformManager* _uniformManager, uint32_t _frameIndex)
{
	if (_ecs->GetComponentIsDirty<DirLight>() || !m_init)
	{
		std::vector<DirLight> dirLights = _ecs->GetComponents<DirLight>();

		m_lightData.nbDirLight = static_cast<uint32_t>(m_dirLightsCache.size());
		m_dirLightsCache.resize(m_lightData.nbDirLight);

		if (m_dirLightsCache.empty())
		{
			m_dirLightsCache.emplace_back();
		}
		else
		{
			for (uint32_t i = 0; i < m_lightData.nbDirLight; ++i)
			{
				m_dirLightsCache[i].dir = dirLights[i].dir;
				m_dirLightsCache[i].ambient = dirLights[i].ambient;
				m_dirLightsCache[i].diffuse = dirLights[i].diffuse;
				m_dirLightsCache[i].specular = dirLights[i].specular;
			}	
		}

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsDirLight[i] = false;
		}

		_ecs->SetComponentIsDirty<DirLight>(false);
	}

	if (_ecs->GetComponentIsDirty<PointLight>() || !m_init)
	{
		std::vector<PointLight> pointLights = _ecs->GetComponents<PointLight>();

		m_lightData.nbPointLight = static_cast<uint32_t> (m_pointLightsCache.size());
		m_pointLightsCache.resize(m_lightData.nbPointLight);

		if (m_pointLightsCache.empty())
		{
			m_pointLightsCache.emplace_back();
		}
		else
		{
			for (uint32_t i = 0; i < m_lightData.nbPointLight; ++i)
			{
				m_pointLightsCache[i].pos = pointLights[i].pos;
				m_pointLightsCache[i].ambient = pointLights[i].ambient;
				m_pointLightsCache[i].diffuse = pointLights[i].diffuse;
				m_pointLightsCache[i].specular = pointLights[i].specular;

				m_pointLightsCache[i].constant = pointLights[i].constant;
				m_pointLightsCache[i].linear = pointLights[i].linear;
				m_pointLightsCache[i].quadratic = pointLights[i].quadratic;

				m_pointLightsCache[i].radius = pointLights[i].radius;
			}
		}

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsPointLight[i] = false;
		}

		_ecs->SetComponentIsDirty<PointLight>(false);
	}

	if (_ecs->GetComponentIsDirty<SpotLight>() || !m_init)
	{
		std::vector<SpotLight> spotLights = _ecs->GetComponents<SpotLight>();

		m_lightData.nbSpotLight = static_cast<uint32_t>(m_spotLightsCache.size());
		m_spotLightsCache.resize(m_lightData.nbSpotLight);

		if (m_spotLightsCache.empty())
		{
			m_spotLightsCache.emplace_back();
		}
		else
		{
			for (uint32_t i = 0; i < m_lightData.nbSpotLight; ++i)
			{
				m_spotLightsCache[i].pos = spotLights[i].pos;
				m_spotLightsCache[i].dir = spotLights[i].dir;

				m_spotLightsCache[i].ambient = spotLights[i].ambient;
				m_spotLightsCache[i].diffuse = spotLights[i].diffuse;
				m_spotLightsCache[i].specular = spotLights[i].specular;

				m_spotLightsCache[i].cutOff = spotLights[i].cutOff;
				m_spotLightsCache[i].outerCutfOff = spotLights[i].outerCutOff;
			}
		}

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsSpotLight[i] = false;
		}

		_ecs->SetComponentIsDirty<SpotLight>(false);
	}

	if (!m_frameSendsDirLight[_frameIndex])
	{
		_uniformManager->StoreData("dirLights", m_dirLightsCache.data(), 
			ARRAY_SIZE_IN_BYTES(m_dirLightsCache), _frameIndex);

		m_frameSendsDirLight[_frameIndex] = true;
	}

	if (!m_frameSendsPointLight[_frameIndex])
	{
		_uniformManager->StoreData("pointLights", m_pointLightsCache.data(),
			ARRAY_SIZE_IN_BYTES(m_pointLightsCache), _frameIndex);

		m_frameSendsPointLight[_frameIndex] = true;
	}

	if (!m_frameSendsSpotLight[_frameIndex])
	{
		_uniformManager->StoreData("spotLights", m_spotLightsCache.data(),
			ARRAY_SIZE_IN_BYTES(m_spotLightsCache), _frameIndex);

		m_frameSendsSpotLight[_frameIndex] = true;
	}

	_uniformManager->StoreData("lightData", &m_lightData, sizeof(LightData), _frameIndex);

	m_init = true;
}