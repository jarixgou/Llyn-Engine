#include "HierarchyInterface.h"

#include "../Scene.h"
#include "../Asset/RessourceManager.h"
#include "../Entities/ECS.h"
#include "../Entities/Entity.h"
#include "../Component/Transform.h"
#include "../ImGui/imgui.h"
#include "../Render/PreviewTexture.h"
#include "../Render/Vulkan/VK_Buffer.h"
#include "../Render/Vulkan/VK_Texture.h"

#define ICON_SIZE 16

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
			ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_FramePadding;

	if (m_selectedEntity == _entity)
	{
		treeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	const size_t childsCount = _entity->childs.size();

	if (childsCount == 0)
	{
		treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}


	ImTextureID texID = (ImTextureID)PreviewTexture::Get().GetPreview("Assets/Core/Textures/Icons/EntityIcon.png")->descriptor;

	const ImGuiStyle& style = ImGui::GetStyle();

	const float padY = style.FramePadding.y + (ICON_SIZE - ImGui::GetFontSize()) * 0.5f;

	const ImVec2 nodePos = ImGui::GetCursorScreenPos();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, padY));
	isOpen = ImGui::TreeNodeEx((void*)(intptr_t)_entity->id, treeFlags, "");
	ImGui::PopStyleVar();

	bool clicked = ImGui::IsItemClicked() || ImGui::IsItemFocused();
	float rowH = ImGui::GetItemRectSize().y;
	float x = nodePos.x + ImGui::GetTreeNodeToLabelSpacing();

	ImGui::SetCursorScreenPos({ x, nodePos.y + (rowH - ICON_SIZE) * 0.5f });
	ImGui::Image(texID, { ICON_SIZE, ICON_SIZE });

	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ICON_SIZE - ImGui::GetFontSize()) * 0.5f);
	ImGui::TextUnformatted(_entity->name.c_str());

	if (clicked)
	{
		m_selectedEntity = _entity;
	}

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
