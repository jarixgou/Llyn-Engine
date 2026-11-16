#include "Editor.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_vulkan.h"

#include "../Engine.h"

void Editor::Init(Engine* _engine)
{
	m_engine = _engine;
}

void Editor::Update()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplVulkan_NewFrame();

	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

	bool demo = false;
	ImGui::ShowDemoWindow(&demo);

	ImGui::Render();
}

void Editor::Cleanup()
{

}