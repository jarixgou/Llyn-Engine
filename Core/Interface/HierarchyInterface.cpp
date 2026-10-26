#include "HierarchyInterface.h"

#include "../Scene.h"
#include "../Entities/ECS.h"
#include "../Entities/Entity.h"
#include "../Component/Transform.h"
#include "../ImGui/imgui.h"

void HierarchyInterface::Init(Scene* _scene)
{
	m_scene = _scene;
}

void HierarchyInterface::Update()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		ImGui::OpenPopup("NewEntity");
	}

	if (ImGui::BeginPopup("NewEntity"))
	{
		if (ImGui::MenuItem("New entity"))
		{
			Entity entity{};
			entity.id = m_scene->ecs->CreateEntity();

			Transform transform{};
			m_scene->ecs->AddComponents(entity.id, transform);

			m_scene->hierarchy.emplace_back(entity);
		}

		ImGui::EndPopup();
	}

	ImGui::Text("Root nodes : %zu", m_scene->hierarchy.size());

	ImGui::Separator();

	ImGui::BeginTable("##list", 1, ImGuiTableFlags_RowBg);

	for (int i = 0; i < m_scene->hierarchy.size(); ++i)
	{
		DrawTreeNode(&m_scene->hierarchy[i]);
	}

	ImGui::EndTable();

	ImGui::End();
}

Entity* HierarchyInterface::GetSelectedEntity()
{
	return m_selectedEntity;
}

Entity** HierarchyInterface::GetSelectedEntityPtr()
{
	return &m_selectedEntity;
}

void HierarchyInterface::DrawTreeNode(Entity* _entity)
{
	bool isOpen = false;

	ImGui::TableNextRow();
	ImGui::TableNextColumn();
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_NavLeftJumpsToParent | ImGuiTreeNodeFlags_SpanFullWidth | 
			ImGuiTreeNodeFlags_DrawLinesToNodes;

	if (m_selectedEntity == _entity)
	{
		treeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	const size_t childsCount = _entity->childs.size();

	if (childsCount == 0)
	{
		treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	isOpen = ImGui::TreeNodeEx((void*)(intptr_t)_entity->id, treeFlags, _entity->name.c_str());

	if (childsCount == 0)
	{
		isOpen = false;
	}

	if (ImGui::IsItemFocused())
	{
		m_selectedEntity = _entity;
	}

	if (isOpen)
	{
		for (int i = 0; i < childsCount; ++i)
		{
			DrawTreeNode(&_entity->childs[i]);
		}

		ImGui::TreePop();
	}
}
