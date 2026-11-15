#include "AssetVisitor.h"

#include "Folder.h"
#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "RessourceManager.h"
#include "../ImGui/imgui.h"
#include "../Render/PreviewTexture.h"
#include "../Render/Vulkan/VK_Texture.h"

#define WINDOW_CHILD_SIZE 50.f
#define ICON_SIZE 64.f

void AssetVisitor::visit(Texture* _texture)
{
	ImGui::BeginChild(std::to_string(_texture->m_id).c_str(), { WINDOW_CHILD_SIZE, WINDOW_CHILD_SIZE });

	//ImTextureID texID = (ImTextureID)_texture->GetVKTexture()->GetPreviewTexture();
	//ImGui::Image(ImTextureRef(texID), { ICON_SIZE, ICON_SIZE });

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
	const ImTextureID texID = (ImTextureID)PreviewTexture::Get().GetPreview("Assets/Core/Textures/Icons/Folder.png")->descriptor;
	ImGui::Image(ImTextureRef(texID), { ICON_SIZE , ICON_SIZE });
}