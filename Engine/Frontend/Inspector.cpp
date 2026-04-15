#include "Inspector.h"

#include <glm/gtc/quaternion.hpp>

#include "Hierarchy.h"

// Component part
#include "../Asset/AssetManager.h"
#include "../Component/Component.h"
#include "../Component/Transform.h"
#include "../Component/Mesh.h"
#include "../Asset/Material.h"
#include "../Component/DirectLight.h"
#include "../Component/PointLight.h"
#include "../Component/SpotLight.h"

#include "../GameObject/GameObject.h"

#include "../Memory/MemoryHelper.h"

#include "../Utils/Utils.h"

#include "../External/imgui/imgui.h"
#include "../LightManager/SpotLightData.h"
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
				DirectLight* directLight = dynamic_cast<DirectLight*>(component);
				PointLight* pointLight = dynamic_cast<PointLight*>(component);
				SpotLight* spotLight = dynamic_cast<SpotLight*>(component);
				if (mesh != nullptr)
				{
					DrawMesh(mesh);
				}
				if (directLight != nullptr)
				{
					DrawDirectLight(directLight);
				}
				if (pointLight != nullptr)
				{
					DrawPointLight(pointLight);
				}
				if (spotLight != nullptr)
				{
					DrawSpotLight(spotLight);
				}
			}

			ImGui::Separator();
			AddComponent();
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
			size_t componentId = transform->GetID();

			ImGui::Text("Position : ");
			ImGui::SameLine();
			Utils::DrawVec3(&transform->position, false, "P" + std::to_string(componentId));

			ImGui::Text("Rotation : ");
			ImGui::SameLine();
			glm::vec3 angle = glm::degrees(glm::eulerAngles(transform->rotation));
			Utils::DrawVec3(&angle, true, "R" + std::to_string(componentId));
			transform->rotation = glm::quat(glm::radians(angle));

			ImGui::Text("Scale : ");
			ImGui::SameLine();
			Utils::DrawVec3(&transform->scale, false, "S" + std::to_string(componentId));
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

	void Inspector::DrawDirectLight(DirectLight* _light) const
	{
		if (ImGui::CollapsingHeader("Direct Light"))
		{
			size_t componentId = _light->GetID();
			glm::vec3 dir = _light->GetDirection();
			glm::vec3 ambient = _light->GetAmbient();
			glm::vec3 diffuse = _light->GetDiffuse();
			glm::vec3 specular = _light->GetSpecular();

			ImGui::Text("Direction :  ");
			ImGui::SameLine();
			if (Utils::DrawVec3(&dir, false, "D" + std::to_string(componentId)))
			{
				_light->SetDir(dir);
			}

			ImGui::Text("Ambient :    ");
			ImGui::SameLine();
			float aCol[3] = { ambient.x, ambient.y, ambient.z };
			if (ImGui::ColorEdit3(std::string("A" + std::to_string(componentId)).c_str(), aCol, ImGuiColorEditFlags_Float))
			{
				ambient = glm::vec3(aCol[0], aCol[1], aCol[2]);
				_light->SetAmbient(ambient);
			}

			ImGui::Text("Diffuse :    ");
			ImGui::SameLine();
			float diCol[3] = { diffuse.x, diffuse.y, diffuse.z };
			if (ImGui::ColorEdit3(std::string("Di" + std::to_string(componentId)).c_str(), diCol, ImGuiColorEditFlags_Float))
			{
				diffuse = glm::vec3(diCol[0], diCol[1], diCol[2]);
				_light->SetDiffuse(diffuse);
			}


			ImGui::Text("Specular :   ");
			ImGui::SameLine();
			float sCol[3] = { specular.x, specular.y, specular.z };
			if (ImGui::ColorEdit3(std::string("S" + std::to_string(componentId)).c_str(), sCol, ImGuiColorEditFlags_Float))
			{
				specular = glm::vec3(sCol[0], sCol[1], sCol[2]);
				_light->SetSpecular(specular);
			}
		}
	}

	void Inspector::DrawPointLight(PointLight* _light) const
	{
		if (ImGui::CollapsingHeader("Point light"))
		{
			size_t componentId = _light->GetID();

			float constant = _light->GetConstant();
			float linear = _light->GetLinear();
			float quadratic = _light->GetQuadratic();
			glm::vec3 ambient = _light->GetAmbient();
			glm::vec3 diffuse = _light->GetDiffuse();
			glm::vec3 specular = _light->GetSpecular();

			ImGui::Text("Constant : ");
			ImGui::SameLine();
			if (ImGui::DragFloat(std::string("##C" + std::to_string(componentId)).c_str(), &constant, 1.f, 0.f, 1.f))
			{
				_light->SetConstant(constant);
			}

			ImGui::Text("Linear : ");
			ImGui::SameLine();
			if (ImGui::DragFloat(std::string("##L" + std::to_string(componentId)).c_str(), &linear, 1.f, 0.f, 2.f))
			{
				_light->SetLinear(linear);
			}

			ImGui::Text("Quadratic : ");
			ImGui::SameLine();
			if (ImGui::DragFloat(std::string("##Q" + std::to_string(componentId)).c_str(), &quadratic, 1.f, 0.f, 2.f))
			{
				_light->SetQuadratic(quadratic);
			}

			ImGui::Text("Ambient :    ");
			ImGui::SameLine();
			float aCol[3] = { ambient.x, ambient.y, ambient.z };
			if (ImGui::ColorEdit3(std::string("##A" + std::to_string(componentId)).c_str(), aCol, ImGuiColorEditFlags_Float))
			{
				ambient = glm::vec3(aCol[0], aCol[1], aCol[2]);
				_light->SetAmbient(ambient);
			}

			ImGui::Text("Diffuse :    ");
			ImGui::SameLine();
			float diCol[3] = { diffuse.x, diffuse.y, diffuse.z };
			if (ImGui::ColorEdit3(std::string("##Di" + std::to_string(componentId)).c_str(), diCol, ImGuiColorEditFlags_Float))
			{
				diffuse = glm::vec3(diCol[0], diCol[1], diCol[2]);
				_light->SetDiffuse(diffuse);
			}


			ImGui::Text("Specular :   ");
			ImGui::SameLine();
			float sCol[3] = { specular.x, specular.y, specular.z };
			if (ImGui::ColorEdit3(std::string("##S" + std::to_string(componentId)).c_str(), sCol, ImGuiColorEditFlags_Float))
			{
				specular = glm::vec3(sCol[0], sCol[1], sCol[2]);
				_light->SetSpecular(specular);
			}
		}
	}

	void Inspector::DrawSpotLight(SpotLight* _light) const
	{
		if (ImGui::CollapsingHeader("Spot light"))
		{
			const size_t componentId = _light->GetID();

			float cutOff = _light->GetCutOff();
			float outerCutOff = _light->GetOuterCutOff();
			glm::vec3 dir = _light->GetDirection();
			glm::vec3 ambient = _light->GetAmbient();
			glm::vec3 diffuse = _light->GetDiffuse();
			glm::vec3 specular = _light->GetSpecular();

			ImGui::Text("Cut off : ");
			ImGui::SameLine();
			if (ImGui::DragFloat(std::string("##C" + std::to_string(componentId)).c_str(), &cutOff, 1.f, 0.f, FLT_MAX))
			{
				_light->SetCutOff(cutOff);
			}

			ImGui::Text("Outer cut off : ");
			ImGui::SameLine();
			if (ImGui::DragFloat(std::string("##O" + std::to_string(componentId)).c_str(), &outerCutOff, 1.f, 0.f, FLT_MAX))
			{
				_light->SetOuterCutOff(outerCutOff);
			}

			ImGui::Text("Direction :  ");
			ImGui::SameLine();
			if (Utils::DrawVec3(&dir, false, "D" + std::to_string(componentId)))
			{
				_light->SetDirection(dir);
			}

			ImGui::Text("Ambient :    ");
			ImGui::SameLine();
			float aCol[3] = { ambient.x, ambient.y, ambient.z };
			if (ImGui::ColorEdit3(std::string("##A" + std::to_string(componentId)).c_str(), aCol, ImGuiColorEditFlags_Float))
			{
				ambient = glm::vec3(aCol[0], aCol[1], aCol[2]);
				_light->SetAmbient(ambient);
			}

			ImGui::Text("Diffuse :    ");
			ImGui::SameLine();
			float diCol[3] = { diffuse.x, diffuse.y, diffuse.z };
			if (ImGui::ColorEdit3(std::string("##Di" + std::to_string(componentId)).c_str(), diCol, ImGuiColorEditFlags_Float))
			{
				diffuse = glm::vec3(diCol[0], diCol[1], diCol[2]);
				_light->SetDiffuse(diffuse);
			}


			ImGui::Text("Specular :   ");
			ImGui::SameLine();
			float sCol[3] = { specular.x, specular.y, specular.z };
			if (ImGui::ColorEdit3(std::string("##S" + std::to_string(componentId)).c_str(), sCol, ImGuiColorEditFlags_Float))
			{
				specular = glm::vec3(sCol[0], sCol[1], sCol[2]);
				_light->SetSpecular(specular);
			}
		}
	}

	void Inspector::AddComponent() const
	{
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

			if (ImGui::MenuItem("Direct light"))
			{
				(*m_selectedGo)->AddComponent(new DirectLight());
			}

			if (ImGui::MenuItem("Point light"))
			{
				(*m_selectedGo)->AddComponent(new PointLight());
			}

			if (ImGui::MenuItem("Spot light"))
			{
				(*m_selectedGo)->AddComponent(new SpotLight());
			}

			ImGui::EndPopup();
		}
	}
}