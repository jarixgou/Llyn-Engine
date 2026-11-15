#include "AssetExplorerInterface.h"

#include <filesystem>

#include "../Asset/Folder.h"
#include "../Asset/RessourceManager.h"
#include "../ImGui/imgui.h"

void AssetExplorerInterface::Init()
{
	const std::string path = "Assets/Core";
	m_actualFolder = new Folder;
	m_actualFolder->path = path;

	RecursiveFolder(m_actualFolder, path);
}

void AssetExplorerInterface::Update()
{
	ImGui::Begin("Asset Explorer");

	ImGui::Columns(m_actualFolder->m_assets.size());

	for (auto & asset : m_actualFolder->m_assets)
	{
		asset->Accept(&m_assetVisitor);
		ImGui::NextColumn();
	}
	ImGui::Columns();

	ImGui::End();
}

void AssetExplorerInterface::Cleanup()
{

}

void AssetExplorerInterface::RecursiveFolder(Folder* _actualFolder, const std::string& _path)
{


	for (const auto& entry : std::filesystem::directory_iterator(_path))
	{
		if (entry.is_directory())
		{
			LOGGER_INFO(entry.path().string());

			Folder* newFolder = new Folder;
			newFolder->previousFolder = _actualFolder;
			newFolder->path = entry.path().string();

			_actualFolder->m_assets.emplace_back(newFolder);

			RecursiveFolder(newFolder, entry.path().string());
		}
		else if (entry.is_regular_file())
		{
			BaseAssetVisitor* asset = nullptr;

			if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg")
			{
				asset = RessourceManager::Get().GetRessource<Texture>(entry.path().string());
			}
			else if (entry.path().extension() == ".mat")
			{
				asset = RessourceManager::Get().GetRessource<Material>(entry.path().string());
			}
			else if (entry.path().extension() == ".gltf")
			{
				asset = RessourceManager::Get().GetRessource<Model>(entry.path().string());
			}

			_actualFolder->m_assets.emplace_back(asset);
		}
	}
}