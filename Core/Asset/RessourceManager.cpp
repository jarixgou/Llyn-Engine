#include "RessourceManager.h"

#include "../Render/UniformManager.h"

RessourceManager& RessourceManager::Get()
{
	static RessourceManager instance;
	return instance;
}

void RessourceManager::Init()
{
	m_textureCaches.clear();
	m_materialCaches.clear();

	m_frameSendsTexture.resize(MAX_FRAMES_IN_FLIGHT, false);
	m_frameSendsMaterial.resize(MAX_FRAMES_IN_FLIGHT, false);
}

void RessourceManager::UpdateRessources(IUniformManager* _uniformManager, uint32_t _framIndex)
{
	if (RessourceIsDirty<Texture>())
	{
		m_textureCaches = GetRessources<Texture>();

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsTexture[i] = false;
		}

		SetRessourceIsDirty<Texture>(false);
	}

	if (RessourceIsDirty<Material>() || !m_init)
	{
		std::vector<Material> materials = GetRessources<Material>();
		m_materialCaches.resize(materials.size());
		for (int i = 0; i < materials.size(); ++i)
		{
			m_materialCaches[i].baseID = materials[i].GetBase();
			m_materialCaches[i].specularID = materials[i].GetSpecular();
			m_materialCaches[i].normalID = materials[i].GetNormal();

			m_materialCaches[i].baseColor = materials[i].GetBaseColor();
			m_materialCaches[i].specularColor = materials[i].GetSpecularColor();

			m_materialCaches[i].smoothness = materials[i].GetSmoothness();
		}

		if (m_materialCaches.empty())
		{
			m_materialCaches.emplace_back();
		}

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSendsMaterial[i] = false;
		}

		SetRessourceIsDirty<Material>(false);
	}

	if (!m_frameSendsTexture[_framIndex])
	{
		_uniformManager->StoreTextures(m_textureCaches, _framIndex);

		m_frameSendsTexture[_framIndex] = true;
	}

	if (!m_frameSendsMaterial[_framIndex])
	{
		_uniformManager->StoreData("materials", m_materialCaches.data(),
			ARRAY_SIZE_IN_BYTES(m_materialCaches), _framIndex);

		m_frameSendsMaterial[_framIndex] = true;
	}

	m_init = true;
}

void RessourceManager::UnloadAll()
{
	for (auto& ressourceHandle : m_ressourcesHandle)
	{
		for (auto& ressource : ressourceHandle.second.ressources)
		{
			if (ressource != nullptr)
			{
				ressource->Unload();

				delete ressource;
				ressource = nullptr;
			}
		}
		ressourceHandle.second.ressources.clear();
		ressourceHandle.second.ressourcesID.clear();
		ressourceHandle.second.ressourcesKey.clear();
	}
	m_ressourcesHandle.clear();
}
