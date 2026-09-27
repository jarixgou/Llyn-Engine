#ifndef INSPECTOR_INTERFACE__H
#define INSPECTOR_INTERFACE__H

#include "../LlynCore.h"

class InspectorInterface
{
public:
	static void Update(Entity* _entity, ECS* _ecs);
private:
	static void DrawTransform(EntityID _id, Transform* _transform);
	static void DrawMeshRender(EntityID _id, MeshRender* _meshRender);
	static void DrawMeshFilter(EntityID _id, MeshFilter* _meshFilter);
	static bool DrawDirLight(DirLight* _dirLight);
	static bool DrawPointLight(PointLight* _pointLight);
	static bool DrawSpotLight(SpotLight* _spotLight);

	static void AddComponent(Entity* _entity, ECS* _ecs);
};

#endif