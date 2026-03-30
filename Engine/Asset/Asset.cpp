#include "Asset.h"

namespace Llyn
{
	void Asset::SetPath(const char* _path)
	{
		m_path = _path;
	}

	std::string Asset::GetPath()
	{
		return m_path;
	}
}
