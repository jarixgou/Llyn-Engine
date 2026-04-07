#include "Inspector.h"

#include <glm/gtc/quaternion.hpp>

#include "Hierarchy.h"

// Component part
#include "../Asset/AssetManager.h"
#include "../Component/Component.h"
#include "../Component/Transform.h"
#include "../Component/Mesh.h"
#include "../Asset/Material.h"

#include "../GameObject/GameObject.h"

#include "../Memory/MemoryHelper.h"

#include "../Utils/Utils.h"

#include "../External/imgui/imgui.h"
#include "../Mesh/MeshFilter.h"
#include "../Mesh/MeshFilterPooler.h"

namespace Llyn
{
	Inspector* Inspector::s_instance = nullptr;

	Inspector::Inspector()
	{
		m_selectedGo = Hierarchy::Get()->GetSelectedGameObjectPtr();
		m_pathsMaterial = Utils::GetFilePathsByExtension(".mat");
	}

	Inspector::~Inspector()
	{
		m_selectedGo = nullptr;
	}

	Inspector* Inspector::Get()
	{
		if (s_instance == nullptr)
		{
			ALLOCATE_MEMORY(s_instance);
		}

		return s_instance;
	}

	void Inspector::Draw() const
	{
		ImGui::Begin("Inspector");

		if (*m_selectedGo != nullptr)
		{
			char nameBuffer[256];
			std::string name = (*m_selectedGo)->GetName();
			std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", name.c_str());
			if (ImGui::InputText("##name", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				(*m_selectedGo)->SetName(nameBuffer);
			}

			ImGui::Separator();

			DrawTransform();

			for (Component* component : *(*m_selectedGo)->GetComponents())
			{
				Mesh* mesh = dynamic_cast<Mesh*>(component);
				if (mesh != nullptr)
				{
					DrawMesh(mesh);
				}
			}

			ImGui::Separator();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - 50);

			if (ImGui::Button("Add component"))
			{
				ImGui::OpenPopup("AddComponentPopup");
			}

			if (ImGui::BeginPopup("AddComponentPopup"))
			{
				if (ImGui::MenuItem("Mesh"))
				{
					(*m_selectedGo)->AddComponent(new Mesh(MeshFilterPooler::Get()->GetMesh("Cube"), nullptr));
				}

				ImGui::EndPopup();
			}
		}
		else
		{
			ImGui::Text("Please select a Game Object !");
		}

		ImGui::End();
	}

	void Inspector::DrawTransform() const
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			Transform* transform = (*m_selectedGo)->GetTransform();

			ImGui::Text("Position : ");
			ImGui::SameLine();
			Utils::DrawVec3(&transform->position, false, "P" + std::to_string(transform->GetID()));

			ImGui::Text("Rotation : ");
			ImGui::SameLine();
			glm::vec3 angle = glm::degrees(glm::eulerAngles(transform->rotation));
			Utils::DrawVec3(&angle, true, "R" + std::to_string(transform->GetID()));
			transform->rotation = glm::quat(glm::radians(angle));

			ImGui::Text("Scale : ");
			ImGui::SameLine();
			Utils::DrawVec3(&transform->scale, false, "S" + std::to_string(transform->GetID()));
		}
	}

	void Inspector::DrawMesh(Mesh* _mesh) const
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{
			std::vector<std::string> allMeshes = MeshFilterPooler::Get()->GetAllNameMeshes();
			MeshFilter* meshFilter = _mesh->GetMeshFilter();

			int indexMeshFilter = -1;
			for (size_t i = 0; i < allMeshes.size(); ++i)
			{
				if (allMeshes[i] == meshFilter->name)
				{
					indexMeshFilter = i;
					i = allMeshes.size();
				}
			}

			std::vector<const char*> meshItems;
			meshItems.reserve(allMeshes.size());
			for (const auto& meshName : allMeshes)
			{
				meshItems.push_back(meshName.c_str());
			}

			ImGui::Text("Filter :");
			ImGui::SameLine();
			if (ImGui::Combo("##MeshFilterCombo", &indexMeshFilter, meshItems.data(), meshItems.size()))
			{
				if (indexMeshFilter >= 0 && indexMeshFilter < allMeshes.size())
				{
					if (allMeshes[indexMeshFilter] != meshFilter->name)
					{
						_mesh->SetMeshFilter(MeshFilterPooler::Get()->GetMesh(meshItems[indexMeshFilter]));
					}
				}
			}

			Material* material = _mesh->GetMaterial();
			ImGui::Text("Material :");
			ImGui::SameLine();
			int idxMaterial = -1;
			for (size_t i = 0; i < m_pathsMaterial.size(); ++i)
			{
				if (m_pathsMaterial[i] == material->GetPath())
				{
					idxMaterial = static_cast<int>(i);
					i = m_pathsMaterial.size();
				}
			}

			std::vector<const char*> materialItems;
			materialItems.reserve(m_pathsMaterial.size());
			for (const auto& path : m_pathsMaterial)
			{
				materialItems.push_back(path.c_str());
			}

			if (ImGui::Combo("##MaterialCombo", &idxMaterial, materialItems.data(), materialItems.size()))
			{
				if (idxMaterial >= 0 && idxMaterial < m_pathsMaterial.size())
				{
					if (m_pathsMaterial[idxMaterial] != material->GetPath())
					{
						_mesh->SetMaterial(AssetManager::Get()->GetAsset<Material>(materialItems[idxMaterial]));
					}
				}
			}
		}
	}
}
