#ifndef LIGHT_MANAGER__H
#define LIGHT_MANAGER__H

#include "../../LlynCore.h"

#include "../../Render/GPUDirLight.h"
#include "../../Render/GPUPointLight.h"
#include "../../Render/GPUSpotLight.h"

#include "DirLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LightData.h"

class LightManager
{
private:
	bool m_init = false;

	std::vector<GPUDirLight> m_dirLightsCache;
	std::vector<bool> m_frameSendsDirLight;

	std::vector<GPUPointLight> m_pointLightsCache;
	std::vector<bool> m_frameSendsPointLight;

	std::vector<GPUSpotLight> m_spotLightsCache;
	std::vector<bool> m_frameSendsSpotLight;

	LightData m_lightData;
public:
	void Init();
	void UpdateLights(ECS* _ecs, IUniformManager* _uniformManager, uint32_t _frameIndex);
};

#endif