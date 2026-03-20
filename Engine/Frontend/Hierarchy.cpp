#include "Hierarchy.h"

#include "../External/imgui/imgui.h"
#include "../GameObject/GameObject.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	Hierarchy* Hierarchy::s_instance = nullptr;

	Hierarchy::Hierarchy()
	{
		m_goList = nullptr;
		m_selectedGo = nullptr;
	}

	Hierarchy::~Hierarchy()
	{
		m_goList = nullptr;
		m_selectedGo = nullptr;
	}

	Hierarchy* Hierarchy::Get()
	{
		if (s_instance == nullptr)
		{
			ALLOCATE_MEMORY(s_instance);
		}

		return s_instance;
	}

	void Hierarchy::Draw()
	{
		ImGui::Begin("Hierarchy");

		ImGui::Text("Root nodes : %zu", m_goList->size());
		ImGui::Separator();
		ImGui::BeginTable("##list", 1, ImGuiTableFlags_RowBg);
		for (auto& go : *m_goList)
		{
			if (DrawTreeNode(go))
			{
				DrawTree(go);
				ImGui::TreePop();
			}
		}
		ImGui::EndTable();

		ImGui::End();
	}

	void Hierarchy::SetGameObjectList(std::vector<GameObject*>* _goList)
	{
		m_goList = _goList;
	}

	GameObject* Hierarchy::GetSelectedGameObject() const
	{
		return m_selectedGo;
	}

	GameObject** Hierarchy::GetSelectedGameObjectPtr()
	{
		return &m_selectedGo;
	}

	void Hierarchy::DrawTree(GameObject* _rootNode)
	{
		if (_rootNode != nullptr)
		{
			for (auto& child : _rootNode->GetChild())
			{
				if (DrawTreeNode(child))
				{
					DrawTree(child);
					ImGui::TreePop();
				}
			}
		}
	}

	bool Hierarchy::DrawTreeNode(GameObject* _go)
	{
		if (_go != nullptr)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_None;
			treeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			treeFlags |= ImGuiTreeNodeFlags_NavLeftJumpsToParent;
			treeFlags |= ImGuiTreeNodeFlags_SpanFullWidth;
			treeFlags |= ImGuiTreeNodeFlags_DrawLinesToNodes;

			const size_t goCount = _go->GetChildCount();

			if (_go == m_selectedGo)
			{
				treeFlags |= ImGuiTreeNodeFlags_Selected;
			}
			if (goCount == 0)
			{
				treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			}

			bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)_go->GetID(), treeFlags, "%s", _go->GetName().c_str());
			if (goCount == 0)
			{
				isOpen = false;
			}
			if (ImGui::IsItemFocused())
			{
				m_selectedGo = _go;
			}

			return isOpen;
		}
		return false;
	}
}
