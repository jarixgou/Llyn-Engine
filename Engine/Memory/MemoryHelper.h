#ifndef MEMORY_HELPER__H
#define MEMORY_HELPER__H

#include <utility>
#include <string>
#include <typeinfo>

#include "../Logger/Logger.h"

namespace Llyn
{
	template<typename T>
	void DeleteMemory(T** _ptr, const char* _file, int _line)
	{
		if (_ptr != nullptr)
		{
			if (*_ptr != nullptr)
			{
				std::string content = "Delete " + std::string(typeid(**_ptr).name()) +
					" size(" + std::to_string(sizeof(**_ptr)) + ")";
				LOG_MEMORY(content.c_str(), _file, _line);

				delete* _ptr;
				*_ptr = nullptr;
			}
		}
	}

	/**
	 *
	 * @param x Variable do you want delete
	 */
#define DELETE_MEMORY(x) \
	DeleteMemory(&x, __FILE__, __LINE__)

	template<typename T>
	void DeleteArrayMemory(T** _ptr, const char* _file, int _line)
	{
		if (_ptr != nullptr)
		{
			if (*_ptr != nullptr)
			{
				std::string content = "Delete " + std::string(typeid(**_ptr).name()) +
					" size(" + std::to_string(sizeof(**_ptr)) + ")";
				LOG_MEMORY(content.c_str(), _file, _line);

				delete[](*_ptr);
				*_ptr = nullptr;
			}
		}
	}

	/**
	 *
	 * @param x Variable do you want delete
	 */
#define DELETE_ARRAY_MEMORY(x) \
	DeleteArrayMemory(&x, __FILE__, __LINE__)

	template<typename T, typename... Args>
	void AllocateMemory(T** _ptr, const char* _file, int _line, Args&&... _args)
	{
		DeleteMemory(_ptr, _file, _line);

		if (_ptr != nullptr)
		{
			if (*_ptr == nullptr)
			{
				*_ptr = new T(std::forward<Args>(_args)...);

				std::string content = "Allocate " + std::string(typeid(**_ptr).name()) +
					" size(" + std::to_string(sizeof(**_ptr)) + ")";
				LOG_MEMORY(content.c_str(), _file, _line);
			}
		}
	}

	/**
	 *
	 * @param x Variable do you want to allocate
	 */
#define ALLOCATE_MEMORY(x, ...) \
	AllocateMemory(&x, __FILE__, __LINE__, ##__VA_ARGS__)

	template<typename T>
	void AllocateArrayMemory(T** _ptr, size_t _size, const char* _file, int _line)
	{
		DeleteArrayMemory(_ptr, _file, _line);

		if (_ptr != nullptr)
		{
			if (*_ptr == nullptr)
			{
				*_ptr = new T[_size];

				std::string content = "Allocate array " + std::to_string(_size) + " " + std::string(typeid(**_ptr).name()) +
					" size(" + std::to_string(sizeof(**_ptr)) + ")";
				LOG_MEMORY(content.c_str(), _file, _line);
			}
		}
	}

	/**
	 *
	 * @param x Variable do you want to allocate
	 * @param y Size of the array
	 */
#define ALLOCATE_ARRAY_MEMORY(x, y) \
	AllocateArrayMemory(&x, y, __FILE__, __LINE__)

}
#endif