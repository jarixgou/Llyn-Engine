#ifndef COMPONENT_POOL__H
#define COMPONENT_POOL__H
#include "SparseSet.h"

class IComponentPool
{
public:
	virtual ~IComponentPool() = default;
	virtual void Remove(const size_t& _entityID) = 0;
};

template<typename T>
class ComponentPool : public IComponentPool, public SparseSet<T> 
{
public:
	void Remove(const size_t& _entityID) override
	{
		SparseSet<T>::Remove(_entityID);
	}
};

#endif