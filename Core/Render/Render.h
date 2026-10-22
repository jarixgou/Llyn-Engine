#ifndef RENDER__H
#define RENDER__H

#include "../LlynCore.h"

class IRender
{
public:
	virtual ~IRender() = default;
	virtual void Init(IUniformManager** _uniformManger) = 0;
	virtual void Update(Scene* _scene, Camera* _camera, LightManager* _lightManager, IUniformManager* _uniformManager) = 0;
	virtual void Draw(Scene* _scene, Camera* _camera, Transform* _cemeraTransform, LightManager* _lightManager, IUniformManager* _uniformManager) = 0;
	virtual void Cleanup() = 0;
};

#endif