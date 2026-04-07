#ifndef ASSET_MANAGER__H
#define ASSET_MANAGER__H

#include <iostream>
#include <string>
#include <unordered_map>

#include "Asset.h"
#include "../Assert/Assert.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	class AssetManager
	{
	private:
		std::unordered_map<std::string, Asset*> m_assets;

		static AssetManager* s_singleton;
	public:

		AssetManager() = default;
		~AssetManager();

		// Singleton
		static AssetManager* Get();

		template<class T>
		T* GetAsset(const char* _path)
		{
			if (!std::is_base_of_v<Asset, T>)
			{
				return nullptr;
			}
			
			if (m_assets.contains(_path))
			{
				return static_cast<T*>(m_assets[_path]);
			}

			T* newAsset = nullptr;
			ALLOCATE_MEMORY(newAsset);
			if (!newAsset->Load(_path))
			{
				std::cerr << "Failed to load asset: " << _path << std::endl;
				DELETE_MEMORY(newAsset);
				return nullptr;
			}

			newAsset->SetPath(_path);
			m_assets.insert({ _path, newAsset });
			return newAsset;
		}

		void Clear();
		bool Unload(const char* _path);
		size_t GetAssetCount() const;
	};
}

#endif