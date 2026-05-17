#ifndef ECS__H
#define ECS__H
#include <queue>
#include <unordered_map>

class ECS
{
private:
	std::unordered_map<size_t, void*> m_componentPools;

	size_t m_componentTypeID = 0;

	std::queue<size_t> freeEntitiesID;
	size_t m_nextEntityID = 0;
};

#endif