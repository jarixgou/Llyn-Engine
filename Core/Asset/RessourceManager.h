#ifndef RESOURCE_MANAGER__H
#define RESOURCE_MANAGER__H

#include <memory>
#include <queue>
#include <unordered_map>

#include "../LlynCore.h"

#include "Ressource.h"
#include "Texture.h"
#include "Material.h"
#include "../Logger.h"
#include "../Render/GPUMaterial.h"

struct RessourceHandle
{
	std::vector<IRessource*> ressources;
	std::unordered_map<std::string, RessourceID> ressourcesID;
	std::unordered_map<RessourceID, std::string> ressourcesKey;

	std::queue<RessourceID> freeIDs;
	bool isDirty = false;
};

class RessourceManager
{
private:
	RessourceManager() = default;
	
	bool m_init = false;
	std::vector<Texture> m_textureCaches;
	std::vector<bool> m_frameSendsTexture;

	std::vector<GPUMaterial> m_materialCaches;
	std::vector<bool> m_frameSendsMaterial;

	std::unordered_map<StringHash, RessourceHandle> m_ressourcesHandle;
public:
	RessourceManager(const RessourceManager&) = delete;
	RessourceManager& operator=(const RessourceManager&) = delete;
	RessourceManager(RessourceManager&&) = delete;
	RessourceManager& operator=(RessourceManager&&) = delete;

	static RessourceManager& Get();

	void Init();

	void UpdateRessources(IUniformManager* _uniformManager, uint32_t _framIndex);

	template<typename T>
	RessourceID GetRessourceID(const std::string& _fileName);

	template<typename T>
	T* GetRessource(const std::string& _fileName);

	template<typename T>
	std::vector<T> GetRessources();

	template<typename T>
	bool RessourceIsDirty();

	template<typename T>
	void SetRessourceIsDirty(bool _dirty);

	template<typename T>
	RessourceID Add(const std::string& _filePath, T* _ressource);

	template<typename T>
	std::string GetFilePath(const RessourceID& _ressourceID);

	template<typename T>
	void Unload(const std::string& _fileName);

	template<typename T>
	void UnloadByID(const RessourceID& _id);

	void UnloadAll();
};

template <typename T>
RessourceID RessourceManager::GetRessourceID(const std::string& _filePath)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];
	if (ressourceHandle.ressourcesID.contains(_filePath))
	{
		if (ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]] != nullptr)
		{
			LOGGER_INFO("Found : " + _filePath);
			return ressourceHandle.ressourcesID[_filePath];
		}
	}

	IRessource* newRessource = new T;
	if (!newRessource->Load(_filePath))
	{
		LOGGER_ERROR("Failed to load : " + _filePath);

		delete newRessource;
		newRessource = nullptr;

		return TOMBSTONE_RESOURCE;
	}

	if (!ressourceHandle.freeIDs.empty())
	{
		const RessourceID nextID = ressourceHandle.freeIDs.front();

		ressourceHandle.ressourcesID[_filePath] = nextID;
		ressourceHandle.ressourcesKey[nextID] = _filePath;
		
		ressourceHandle.ressources[nextID] = newRessource;

		ressourceHandle.freeIDs.pop();

		ressourceHandle.isDirty = true;

		return nextID;
	}

	const RessourceID nextID = static_cast<RessourceID>(ressourceHandle.ressources.size());

	ressourceHandle.ressources.emplace_back(newRessource);
	ressourceHandle.ressourcesID[_filePath] = nextID;
	ressourceHandle.ressourcesKey[nextID] = _filePath;

	ressourceHandle.isDirty = true;

	LOGGER_INFO("Loaded : " + _filePath);

	return nextID;
}

template <typename T>
T* RessourceManager::GetRessource(const std::string& _filePath)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];
	if (ressourceHandle.ressourcesID.contains(_filePath))
	{
		if (ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]] != nullptr)
		{
			LOGGER_INFO("Found : " + _filePath);
			return static_cast<T*>(ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]]);
		}
	}

	IRessource* newRessource = new T;
	if (!newRessource->Load(_filePath))
	{
		LOGGER_ERROR("Failed to load : " + _filePath);

		delete newRessource;
		newRessource = nullptr;

		return nullptr;
	}

	if (!ressourceHandle.freeIDs.empty())
	{
		const RessourceID nextID = ressourceHandle.freeIDs.front();

		ressourceHandle.ressources[nextID] = newRessource;
		ressourceHandle.ressourcesID[_filePath] = nextID;
		ressourceHandle.ressourcesKey[nextID] = _filePath;

		ressourceHandle.ressources[nextID] = newRessource;

		ressourceHandle.freeIDs.pop();

		ressourceHandle.isDirty = true;

		return static_cast<T*>(newRessource);
	}

	const RessourceID nextID = ressourceHandle.ressources.size();

	ressourceHandle.ressources.emplace_back(newRessource);
	ressourceHandle.ressourcesID[_filePath] = nextID;
	ressourceHandle.ressourcesKey[nextID] = _filePath;

	ressourceHandle.isDirty = true;

	LOGGER_INFO("Loaded : " + _filePath);

	return static_cast<T*>(newRessource);
}

template <typename T>
std::vector<T> RessourceManager::GetRessources()
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];

	std::vector<T> list(ressourceHandle.ressources.size());
	for (int i = 0; i < ressourceHandle.ressources.size(); ++i)
	{
		list[i] = *static_cast<T*>(ressourceHandle.ressources[i]);
	}

	return list;
}

template <typename T>
bool RessourceManager::RessourceIsDirty()
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");
	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];

	return ressourceHandle.isDirty;
}

template <typename T>
void RessourceManager::SetRessourceIsDirty(bool _dirty)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");
	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];

	ressourceHandle.isDirty = _dirty;
}

template<typename T> 
RessourceID RessourceManager::Add(const std::string& _filePath, T* _ressource)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	if (_ressource == nullptr)
	{
		return TOMBSTONE_RESOURCE;
	}

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];
	if (ressourceHandle.ressourcesID.contains(_filePath))
	{
		// TODO: print error message with logger
		return TOMBSTONE_RESOURCE;
	}

	IRessource* newRessource = new T;
	if (!newRessource->Add(_filePath, _ressource))
	{
		LOGGER_ERROR("Failed to add : " + _filePath);

		delete newRessource;
		newRessource = nullptr;

		return TOMBSTONE_RESOURCE;
	}

	if (!ressourceHandle.freeIDs.empty())
	{
		const RessourceID nextID = ressourceHandle.freeIDs.front();

		ressourceHandle.ressources[nextID] = newRessource;
		ressourceHandle.ressourcesID[_filePath] = nextID;
		ressourceHandle.ressourcesKey[nextID] = _filePath;

		ressourceHandle.freeIDs.pop();

		return nextID;
	}

	const RessourceID nextID = static_cast<RessourceID>(ressourceHandle.ressources.size());

	ressourceHandle.ressources.emplace_back(newRessource);
	ressourceHandle.ressourcesID[_filePath] = nextID;
	ressourceHandle.ressourcesKey[nextID] = _filePath;

	LOGGER_INFO("Added : " + _filePath);
	return nextID;
}

template<typename T>
std::string RessourceManager::GetFilePath(const RessourceID& _ressourceID)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];

	if (!ressourceHandle.ressourcesKey.contains(_ressourceID))
	{
		return std::string();
	}

	return ressourceHandle.ressourcesKey[_ressourceID];
}

template <typename T>
void RessourceManager::Unload(const std::string& _filePath)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];

	if (ressourceHandle.ressourcesID.contains(_filePath))
	{
		if (ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]] != nullptr)
		{
			ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]]->Unload();

			delete ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]];
			ressourceHandle.ressources[ressourceHandle.ressourcesID[_filePath]] = nullptr;

			LOGGER_INFO("Unload : " + _filePath);
		}

		ressourceHandle.freeIDs.push(ressourceHandle.ressourcesID[_filePath]);
		ressourceHandle.ressourcesID[_filePath] = TOMBSTONE_RESOURCE;
	}
}

template <typename T>
void RessourceManager::UnloadByID(const RessourceID& _id)
{
	static_assert(std::is_base_of_v<IRessource, T>, "T must derive from IRessource");

	if (_id == TOMBSTONE_RESOURCE)
	{
		LOGGER_WARNING("Not a valid RessourceID");
		return;
	}

	RessourceHandle& ressourceHandle = m_ressourcesHandle[GET_TYPE_SIGNATURE(T)];
	if (ressourceHandle.ressourcesKey.contains(_id))
	{
		const std::string key = ressourceHandle.ressourcesKey[_id];
		if (ressourceHandle.ressourcesID.contains(key))
		{
			if (ressourceHandle.ressources[ressourceHandle.ressourcesID[key]] != nullptr)
			{
				ressourceHandle.ressources[ressourceHandle.ressourcesID[key]]->Unload();

				delete ressourceHandle.ressources[ressourceHandle.ressourcesID[key]];
				ressourceHandle.ressources[ressourceHandle.ressourcesID[key]] = nullptr;

				LOGGER_INFO("Unload : " + key);
			}

			ressourceHandle.freeIDs.push(ressourceHandle.ressourcesID[key]);
			ressourceHandle.ressourcesID[key] = TOMBSTONE_RESOURCE;
		}
	}
}

#endif