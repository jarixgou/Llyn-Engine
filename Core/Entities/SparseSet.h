#ifndef SPARSE_SET__H
#define SPARSE_SET__H
#include <vector>

#include "../LlynCore.h"

template<typename T>
class SparseSet
{
private:
	std::vector<T> m_dense;
	std::vector<EntityID> m_sparse;

	bool isDirty = false;

public:
	void Insert(const size_t& _entityID, const T& _component);
	T* Get(const size_t& _entityID);
	void Remove(size_t _entityID);

	std::vector<T> GetVector();

	bool GetIsDirty();
	void SetIsDirty(bool _dirty);

	T* Data();
	size_t Size();
};

template <typename T>
void SparseSet<T>::Insert(const size_t& _entityID, const T& _component)
{
	const size_t idx = m_dense.size();
	m_dense.emplace_back(_component);

	if (_entityID >= m_sparse.size())
	{
		m_sparse.resize(_entityID + 1, TOMBSTONE_ENTITY);
		m_sparse[_entityID] = static_cast<EntityID>(idx);
	}
	else
	{
		m_sparse[_entityID] = static_cast<EntityID>(idx);
	}

	isDirty = true;
}

template <typename T>
T* SparseSet<T>::Get(const size_t& _entityID)
{
	if (_entityID >= m_sparse.size())
	{
		return nullptr;
	}

	const size_t idx = m_sparse[_entityID];

	if (idx == TOMBSTONE_ENTITY)
	{
		return nullptr;
	}

	return &m_dense[idx];
}

template <typename T>
void SparseSet<T>::Remove(size_t _entityID)
{
	if (_entityID >= m_sparse.size())
	{
		return;
	}

	const size_t idx = m_sparse[_entityID];

	if (idx == TOMBSTONE_ENTITY)
	{
		return;
	}

	m_dense[idx] = m_dense.back();
	m_dense.pop_back();

	m_sparse[_entityID] = TOMBSTONE_ENTITY;

	isDirty = true;
}

template <typename T>
std::vector<T> SparseSet<T>::GetVector()
{
	return m_dense;
}

template <typename T>
bool SparseSet<T>::GetIsDirty()
{
	return isDirty;
}

template <typename T>
void SparseSet<T>::SetIsDirty(bool _dirty)
{
	isDirty = _dirty;
}

template <typename T>
T* SparseSet<T>::Data()
{
	return m_dense.data();
}

template <typename T>
size_t SparseSet<T>::Size()
{
	return m_dense.size();
}

#endif
