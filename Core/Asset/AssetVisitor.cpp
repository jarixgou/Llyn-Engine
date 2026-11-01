#include "AssetVisitor.h"

#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "Folder.h"
#include "../ImGui/imgui.h"
#include "../Render/Vulkan/VK_Texture.h"

#define WINDOW_CHILD_SIZE 50.f
#define ICON_SIZE 32.f

void AssetVisitor::visit(Texture* _texture)
{
	ImGui::BeginChild(std::to_string(_texture->m_id).c_str(), { WINDOW_CHILD_SIZE, WINDOW_CHILD_SIZE });

	ImTextureID texID = (ImTextureID)_texture->GetVKTexture()->GetPreviewTexture();
	ImGui::Image(ImTextureRef(texID), { ICON_SIZE, ICON_SIZE });

	ImGui::Text(_texture->m_name.c_str());

	ImGui::EndChild();
}

void AssetVisitor::visit(const Material* _material)
{

}

void AssetVisitor::visit(const Model* _model)
{

}

void AssetVisitor::visit(Folder* _folder)
{

}