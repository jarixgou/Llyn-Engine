#ifndef LIGHT_MANAGER__H
#define LIGHT_MANAGER__H

#include "../../LlynCore.h"

#include "DirLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LightData.h"

class LightManager
{
private:
	bool m_init = false;

	std::vector<DirLight> m_dirLightsCache;
	std::vector<bool> m_frameSendsDirLight;

	std::vector<PointLight> m_pointLightsCache;
	std::vector<bool> m_frameSendsPointLight;

	std::vector<SpotLight> m_spotLightsCache;
	std::vector<bool> m_frameSendsSpotLight;

	LightData m_lightData;
public:
	void Init();
	void UpdateLights(ECS* _ecs, IUniformManager* _uniformManager, uint32_t _frameIndex);
};

#endif