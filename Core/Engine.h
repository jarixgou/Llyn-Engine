#ifndef ENGINE__H
#define ENGINE__H

#include <vector>
#include "LlynCore.h"

class Engine
{
private:
	Scene* m_scene = nullptr;
	
	Transform* m_cameraTransform = nullptr;
	Camera* m_cameraEditor = nullptr;

	IRender* m_render = nullptr;

	LightManager* m_lightManager = nullptr;
	IUniformManager* m_uniformManager = nullptr;

	Clock* m_clock = nullptr;
public:
	void Init();
	void Update();
	void Cleanup();
};

#endif