#include "Inspector.h"

#include <glm/gtc/quaternion.hpp>

#include "Hierarchy.h"
#include "../Component/Transform.h"
#include "../External/imgui/imgui.h"
#include "../GameObject/GameObject.h"
#include "../Mesh/Mesh.h"
#include "../Component/IComponent.h"
#include "../Memory/MemoryHelper.h"
#include "../Utils/Utils.h"

namespace Llyn
{
	Inspector* Inspector::s_instance = nullptr;

	Inspector::Inspector()
	{
		m_selectedGo = Hierarchy::Get()->GetSelectedGameObjectPtr();
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

			for (IComponent* component : *(*m_selectedGo)->GetComponents())
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
			
		}
	}
}
