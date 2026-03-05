#include "AssetManager.h"

namespace Llyn
{
	AssetManager* AssetManager::s_singleton = nullptr;

	AssetManager::~AssetManager()
	{
		DELETE_MEMORY(s_singleton);
		Clear();
	}

	AssetManager* AssetManager::Get()
	{
		if (s_singleton == nullptr)
		{
			ALLOCATE_MEMORY(s_singleton);
		}
		return s_singleton;
	}

	void AssetManager::Clear()
	{
		for (auto & asset : m_assets)
		{
			DELETE_MEMORY(asset.second);
		}
		m_assets.clear();
	}

	bool AssetManager::Unload(const char* _path)
	{
		return m_assets.erase(std::string(_path)) > 0;
	}

	size_t AssetManager::GetAssetCount() const
	{
		return m_assets.size();
	}
}
