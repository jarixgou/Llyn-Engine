#ifndef COMPONENT_POOL__H
#define COMPONENT_POOL__H
#include <bitset>

#include "../Entities/SparseSet.h"

#define MAX_COMPONENT 32

using ComponentMask = std::bitset<MAX_COMPONENT>;

class IComponent
{
public:
	virtual ~IComponent() = default;
	virtual void Remove(const size_t& _entityID);
};

inline void IComponent::Remove(const size_t& _entityID)
{
}

template<typename T>
class ComponentPool : public IComponent, public SparseSet<T> 
{
public:
	void Remove(const size_t& _entityID) override
	{
		SparseSet<T>::Remove(_entityID);
	}
};

#endif