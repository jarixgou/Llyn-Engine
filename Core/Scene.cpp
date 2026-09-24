#include "Scene.h"

#include "Entities/ECS.h"
#include "Entities/Entity.h"

void Scene::Init()
{
	ecs = new ECS;
	hierarchy.clear();
}