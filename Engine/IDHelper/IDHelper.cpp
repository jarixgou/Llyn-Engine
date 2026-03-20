#include "IDHelper.h"

namespace Llyn
{
	static size_t id = 0;

	size_t GetNextID()
	{
		return id++;
	}

	size_t GetCurrentID()
	{
		return id;
	}
}
