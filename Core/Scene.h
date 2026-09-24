#ifndef SCENE__H
#define SCENE__H
#include <vector>

#include "Entities/Entity.h"

class ECS;

struct Scene
{
	ECS* ecs = nullptr;
	std::vector<Entity> hierarchy;

	void Init();
};

#endif