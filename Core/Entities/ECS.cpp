#include "ECS.h"

bool ECS::EntityIsValid(const EntityID& _entityID) const
{
	return _entityID < m_nextEntityID;
}

EntityID ECS::CreateEntity()
{
	EntityID newID = 0;

	if (!m_freeEntitiesID.empty())
	{
		newID = m_freeEntitiesID.front();
		m_freeEntitiesID.pop();
		
		m_componentMasks[newID].reset();
	}
	else
	{
		newID = m_nextEntityID++;
		m_componentMasks.emplace_back();
	}

	m_entityCount += 1;

	return newID;
}

void ECS::DestroyEntity(const EntityID& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return;
	}

	for (size_t i = 0; i < m_componentPool.size(); ++i)
	{
		if (m_componentMasks[_entityID].test(i))
		{
			m_componentPool[i]->Remove(_entityID);
		}
	}

	m_entityCount -= 1;

	m_freeEntitiesID.emplace(_entityID);
}

ComponentMask* ECS::GetComponentMask(const EntityID& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return nullptr;
	}

	return &m_componentMasks[_entityID];
}

uint32_t ECS::GetEntityCount()
{
	return m_entityCount;
}
