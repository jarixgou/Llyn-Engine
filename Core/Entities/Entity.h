#ifndef ENTITY__H
#define ENTITY__H

#include <vector>

#include "../LlynCore.h"

struct Entity
{
	std::string name = DEFAULT_ENTITY_NAME;

	EntityID parentID = TOMBSTONE_ENTITY;

	EntityID id = TOMBSTONE_ENTITY;

	std::vector<Entity> childs;
};

#endif