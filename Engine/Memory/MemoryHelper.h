#ifndef MEMORY_HELPER__H
#define MEMORY_HELPER__H

#include <utility>

template<typename T>
void DeleteMemory(T** _ptr)
{
	if (_ptr != nullptr)
	{
		if (*_ptr != nullptr)
		{
			delete *_ptr;
			*_ptr = nullptr;
		}
	}
}

template<typename T>
void DeleteArrayMemory(T** _ptr)
{
	if (_ptr != nullptr)
	{
		if (*_ptr != nullptr)
		{
			delete[]  (*_ptr);
			*_ptr = nullptr;
		}
	}
}

template<typename T, typename... Args>
void AllocateMemory(T** _ptr, Args&&... _args)
{
	DeleteMemory(_ptr);

	if (_ptr != nullptr)
	{
		if (*_ptr == nullptr)
		{
			*_ptr = new T(std::forward<Args>(_args)...);
		}
	}
}

template<typename T>
void AllocateArrayMemory(T** _ptr, size_t _size)
{
	DeleteArrayMemory(_ptr);

	if (_ptr != nullptr)
	{
		if (*_ptr == nullptr)
		{
			*_ptr = new T[_size];
		}
	}
}

#endif