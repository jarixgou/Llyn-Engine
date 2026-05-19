#ifndef ECS__H
#define ECS__H

#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>

#include "Component.h"

class ECS
{
private: // Variable part
	std::array<IComponentPool*, MAX_COMPONENT> m_componentPool{nullptr};

	std::vector<ComponentMask> m_componentMasks;

	static size_t m_nextComponentTypeID;

	std::queue<size_t> m_freeEntitiesID;
	size_t m_nextEntityID = 0;

private: // Private func
	bool EntityIsValid(const size_t& _entityID) const;

	template<typename T>
	static size_t GetComponentTypeID();

	template<typename T>
	ComponentPool<T>& GetPool();
public: // Public func
	size_t CreateEntity();

	template<typename... T>
	static ComponentMask CreateComponentMask();

	template<typename... T>
	void AddComponents(const size_t& _entityID, const T&... _components);

	template<typename T>
	T* GetComponent(const size_t& _entityID);

	template<typename... T>
	void RemoveComponents(const size_t& _entityID);

	void DestroyEntity(const size_t& _entityID);

	template<typename... T>
	bool HasComponent(const size_t& _entityID) const;

	ComponentMask* GetComponentMask(const size_t& _entityID);
};

template <typename T>
size_t ECS::GetComponentTypeID()
{
	static size_t typeID = m_nextComponentTypeID++;
	return typeID;
}

template <typename T>
ComponentPool<T>& ECS::GetPool()
{
	const size_t typeID = GetComponentTypeID<T>();

	const IComponentPool* content = m_componentPool[typeID];
	if (content == nullptr)
	{
		ComponentPool<T>* newComponent = new ComponentPool<T>();
		m_componentPool[typeID] = newComponent;
		return *newComponent;
	}

	return *static_cast<ComponentPool<T>*>(content);
}

template <typename ... T>
ComponentMask ECS::CreateComponentMask()
{
	ComponentMask mask;
	(mask.set(GetComponentTypeID<T>()), ...);
	return mask;
}

template <typename ... T>
void ECS::AddComponents(const size_t& _entityID, const T&... _components)
{
	if (!EntityIsValid(_entityID))
	{
		return;
	}

	(GetPool<T>().Insert(_entityID, _components), ...);
	(m_componentMasks[_entityID].set(GetComponentTypeID<T>()), ...);
}

template <typename T>
T* ECS::GetComponent(const size_t& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return nullptr;
	}

	if (!HasComponent<T>(_entityID))
	{
		return nullptr;
	}

	return GetPool<T>().Get(_entityID);
}

template <typename ... T>
void ECS::RemoveComponents(const size_t& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return;
	}

	(GetPool<T>().Remove(_entityID), ...);
	(m_componentMasks[_entityID].reset(GetComponentTypeID<T>()), ...);
}

template <typename ... T>
bool ECS::HasComponent(const size_t& _entityID) const
{
	if (!EntityIsValid(_entityID))
	{
		return false;
	}

	ComponentMask queryMask;
	(queryMask.set(GetComponentTypeID<T>()), ...);

	return (m_componentMasks[_entityID] & queryMask) == queryMask;
}

#endif