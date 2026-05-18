#include "ECS.h"

size_t ECS::m_nextComponentTypeID = 0;

bool ECS::EntityIsValid(const size_t& _entityID) const
{
	return _entityID < m_nextEntityID;
}

size_t ECS::CreateEntity()
{
	size_t newID = 0;

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

	return newID;
}

void ECS::DestroyEntity(const size_t& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return;
	}

	for (size_t i = 0; i < m_nextComponentTypeID; ++i)
	{
		if (m_componentMasks[_entityID].test(i))
		{
			m_componentPool[i]->Remove(_entityID);
		}
	}

	m_freeEntitiesID.emplace(_entityID);
}

ComponentMask* ECS::GetComponentMask(const size_t& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return nullptr;
	}

	return &m_componentMasks[_entityID];
}