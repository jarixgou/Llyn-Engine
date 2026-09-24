#ifndef ECS__H
#define ECS__H

#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>

#include "../LlynCore.h"
#include "../Component/Component.h"

class ECS
{
private: // Variable part
	std::unordered_map<StringHash, size_t> m_hashComponentID;
	std::vector<IComponent*> m_componentPool;

	std::vector<ComponentMask> m_componentMasks;

	std::queue<EntityID> m_freeEntitiesID;
	EntityID m_nextEntityID = 0;

	uint32_t m_entityCount = 0;

private: // Private func
	bool EntityIsValid(const EntityID& _entityID) const;

	template <typename T>
	size_t GetComponentID();

	template<typename T>
	SparseSet<T>* GetPool();
public: // Public func
	EntityID CreateEntity();

	template<typename... T>
	static ComponentMask CreateComponentMask();

	template<typename... T>
	void AddComponents(const EntityID& _entityID, const T&... _components);

	template<typename T>
	T* GetComponent(const EntityID& _entityID);

	template<typename... T>
	void RemoveComponents(const EntityID& _entityID);

	void DestroyEntity(const EntityID& _entityID);

	template<typename... T>
	bool HasComponent(const EntityID& _entityID);

	ComponentMask* GetComponentMask(const EntityID& _entityID);

	template<typename T>
	std::vector<T> GetComponents();

	template<typename T>
	bool GetComponentIsDirty();

	template<typename T>
	void SetComponentIsDirty(bool _dirty);

	uint32_t GetEntityCount();
};

template <typename T>
size_t ECS::GetComponentID()
{
	StringHash hashCompoentID = GET_TYPE_SIGNATURE(T);

	if (!m_hashComponentID.contains(hashCompoentID))
	{
		m_hashComponentID[hashCompoentID] = m_componentPool.size();
		m_componentPool.push_back(nullptr);
	}
	return m_hashComponentID[hashCompoentID];
}

template <typename T>
SparseSet<T>* ECS::GetPool()
{
	const size_t typeID = GetComponentID<T>();

	IComponent* content = m_componentPool[typeID];
	if (content == nullptr)
	{
		ComponentPool<T>* newComponent = new ComponentPool<T>();
		m_componentPool[typeID] = newComponent;
		return static_cast<SparseSet<T>*>(newComponent);
	}

	return static_cast<SparseSet<T>*>(static_cast<ComponentPool<T>*>(content));
}

template <typename ... T>
ComponentMask ECS::CreateComponentMask()
{
	ComponentMask mask;
	(mask.set(GetComponentID<T>()), ...);
	return mask;
}

template <typename ... T>
void ECS::AddComponents(const EntityID& _entityID, const T&... _components)
{
	if (!EntityIsValid(_entityID))
	{
		return;
	}

	(GetPool<T>()->Insert(_entityID, _components), ...);
	(m_componentMasks[_entityID].set(GetComponentID<T>()), ...);
}

template <typename T>
T* ECS::GetComponent(const EntityID& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return nullptr;
	}

	if (!HasComponent<T>(_entityID))
	{
		return nullptr;
	}

	return GetPool<T>()->Get(_entityID);
}

template <typename ... T>
void ECS::RemoveComponents(const EntityID& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return;
	}

	(GetPool<T>()->Remove(_entityID), ...);
	(m_componentMasks[_entityID].reset(GetComponentID<T>()), ...);
}

template <typename ... T>
bool ECS::HasComponent(const EntityID& _entityID)
{
	if (!EntityIsValid(_entityID))
	{
		return false;
	}

	ComponentMask queryMask;
	(queryMask.set(GetComponentID<T>()), ...);

	return (m_componentMasks[_entityID] & queryMask) == queryMask;
}

template <typename T>
std::vector<T> ECS::GetComponents()
{
	return GetPool<T>()->GetVector();
}

template <typename T>
bool ECS::GetComponentIsDirty()
{
	return GetPool<T>()->GetIsDirty();
}

template <typename T>
void ECS::SetComponentIsDirty(bool _dirty)
{
	GetPool<T>()->SetIsDirty(_dirty);
}

#endif
