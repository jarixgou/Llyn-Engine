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
		m_dirLightsCache = _ecs->GetComponents<DirLight>();

		m_lightData.nbDirLight = static_cast<uint32_t>(m_dirLightsCache.size());

		if (m_dirLightsCache.empty())
		{
			m_dirLightsCache.emplace_back();
		}

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsDirLight[i] = false;
		}

		_ecs->SetComponentIsDirty<DirLight>(false);
	}

	if (_ecs->GetComponentIsDirty<PointLight>() || !m_init)
	{
		m_pointLightsCache = _ecs->GetComponents<PointLight>();

		m_lightData.nbPointLight = static_cast<uint32_t> (m_pointLightsCache.size());

		if (m_pointLightsCache.empty())
		{
			m_pointLightsCache.emplace_back();
		}

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsPointLight[i] = false;
		}

		_ecs->SetComponentIsDirty<PointLight>(false);
	}

	if (_ecs->GetComponentIsDirty<SpotLight>() || !m_init)
	{
		m_spotLightsCache = _ecs->GetComponents<SpotLight>();

		m_lightData.nbSpotLight = static_cast<uint32_t>(m_spotLightsCache.size());

		if (m_spotLightsCache.empty())
		{
			m_spotLightsCache.emplace_back();
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