#ifndef SPARSE_SET__H
#define SPARSE_SET__H
#include <vector>

template<typename T>
class SparseSet
{
private:
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
		m_sparse.resize(_entityID + 1);
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

	const size_t index = m_sparse[_entityID];
	return &m_dense[index];
}

template <typename T>
void SparseSet<T>::Remove(size_t _entityID)
{
	if (_entityID >= m_sparse.size())
	{
		return;
	}

	const size_t idx = m_sparse[_entityID];


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
