#include "VK_Viewport.h"

#include "VK_RenderGraph.h"
#include "../../ImGui/imgui_impl_vulkan.h"

void VK_Viewport::Init(const std::string& _defaultResource, VK_RenderGraph* _renderGraph)
{
	m_actualResource = _renderGraph->GetResource(_defaultResource);
	m_descriptorSet = ImGui_ImplVulkan_AddTexture(m_actualResource->image.m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void VK_Viewport::Update(VK_RenderGraph* _renderGraph)
{
	ImGui::Begin("Viewport");
	
	ImGui::Text("Selected image : ");
	ImGui::SameLine();

	const std::vector<std::string>& resourcesName = _renderGraph->GetResourcesName();
	if (ImGui::BeginCombo("##ImageCombo", m_actualResource->name.c_str()))
	{
		static ImGuiTextFilter filter;
		if (ImGui::IsWindowAppearing())
		{
			filter.Clear();
		}

		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
		filter.Draw("##ImageFilter", -FLT_MIN);

		for (size_t i = 0; i < resourcesName.size(); ++i)
		{
			const bool isSelected = resourcesName[i] == m_actualResource->name;
			if (filter.PassFilter(resourcesName[i].c_str()))
			{
				if (ImGui::Selectable(resourcesName[i].c_str(), isSelected))
				{
					m_actualResource = _renderGraph->GetResource(resourcesName[i]);
					m_descriptorSet = ImGui_ImplVulkan_AddTexture(m_actualResource->image.m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				}
			}
		}

		ImGui::EndCombo();
	}
	
	ImGui::Separator();

	VkCommandBuffer cmdBuff = VK_Buffer::BeginSingleTimeCommand();
	m_actualResource->image.TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VK_Buffer::EndSingleTimeCommand(cmdBuff);

	ImVec2 size = ImGui::GetContentRegionAvail();
	ImTextureID texId = (ImTextureID)m_descriptorSet;
	ImGui::Image(ImTextureRef(texId), size);

	ImGui::End();
}

void VK_Viewport::Cleanup()
{

}