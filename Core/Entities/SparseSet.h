#ifndef SPARSE_SET__H
#define SPARSE_SET__H
#include <vector>

template<typename T>
class SparseSet
{
private:
	static constexpr size_t m_tombstone = std::numeric_limits<size_t>::max();

	std::vector<T> m_dense;
	std::vector<size_t> m_sparse;

public:
	void Insert(const size_t& _entityID, const T& _component);
	T* Get(const size_t& _entityID);
	void Remove(size_t _entityID);

	T* Data();
	size_t Size();
};

template <typename T>
void SparseSet<T>::Insert(const size_t& _entityID, const T& _component)
{
	if (_entityID >= m_sparse.size())
	{
		m_sparse.resize(_entityID + 1, m_tombstone);
	}

	const size_t idx = m_dense.size();
	m_dense.emplace_back(_component);
	m_sparse[_entityID] = idx;
}

template <typename T>
T* SparseSet<T>::Get(const size_t& _entityID)
{
	if (_entityID >= m_sparse.size())
	{
		return nullptr;
	}

	const size_t idx = m_sparse[_entityID];

	if (idx == m_tombstone)
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

	if (idx == m_tombstone)
	{
		return;
	}

	m_dense[idx] = m_dense.back();
	m_dense.pop_back();

	m_sparse[_entityID] = m_tombstone;
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
