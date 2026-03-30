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
			if (std::is_base_of_v<Asset, T>)
			{
				std::string key(_path);

				if (m_assets.contains(key))
				{
					return static_cast<T*>(m_assets[key]);
				}

				T* newAsset = nullptr;
				ALLOCATE_MEMORY(newAsset);
				if (!newAsset->Load(_path))
				{
					std::cerr << "Failed to load asset: " << _path << std::endl;
					DELETE_MEMORY(newAsset);
					return nullptr;
				}

				m_assets.insert({ key, newAsset });
				return newAsset;
			}
			return nullptr;
		}

		void Clear();
		bool Unload(const char* _path);
		size_t GetAssetCount() const;
	};
}

#endif