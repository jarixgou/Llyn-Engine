#ifndef ASSET_MANAGER__H
#define ASSET_MANAGER__H

#include <iostream>
#include <string>
#include <unordered_map>

#include "IAsset.h"
#include "../Assert/Assert.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	class AssetManager
	{
	private:
		std::unordered_map<std::string, IAsset*> m_assets;

		static AssetManager* s_singleton;
	public:
		// Singleton
		static AssetManager* Get();

		template<class T>
		T* GetAsset(const char* _path)
		{
			if (std::is_base_of_v<IAsset, T>)
			{
				std::string key(_path);

				// Si l'asset existe déjà, le retourner
				if (m_assets.find(key) != m_assets.end())
				{
					return static_cast<T*>(m_assets[key]);
				}

				// Créer un nouvel asset
				T* newAsset = nullptr;
				AllocateMemory(&newAsset);
				if (!newAsset->Load(_path))
				{
					std::cerr << "Failed to load asset: " << _path << std::endl;
					DeleteMemory(&newAsset);
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