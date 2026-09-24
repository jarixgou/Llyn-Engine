#include "Engine.h"

#include <iostream>

#include "Clock.h"
#include "Scene.h"
#include "Asset/RessourceManager.h"
#include "Component/MeshFIlterPooler.h"
#include "Render/Vulkan/VK_Render.h"
#include "Render/UniformManager.h"
#include "Asset/Model.h"
#include "Entities/ECS.h"

#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Light/LightManager.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_internal.h"

void Engine::Init()
{
	m_scene = new Scene;
	m_scene->Init();

	m_render = new VK_Render;
	m_render->Init(&m_uniformManager);

	m_lightManager = new LightManager;
	m_lightManager->Init();

	m_clock = new Clock;
	m_clock->Restart();

	MeshFilterPooler::Get().Init();
	RessourceManager::Get().Init();

	m_cameraTransform = new Transform;
	m_cameraTransform->pos.z = -10;
	m_cameraEditor = new Camera;
	m_cameraEditor->UpdateMatrices(*m_cameraTransform);

	Entity entity;
	entity.id = m_scene->ecs->CreateEntity();

	Model* model = RessourceManager::Get().GetRessource<Model>("Core/Models/Sponza/scene.gltf");
	model->SetModel(m_scene->ecs, &entity);
	Transform* transform = m_scene->ecs->GetComponent<Transform>(entity.id);

	m_scene->hierarchy.emplace_back(entity);
}

void Engine::Update()
{
	const float dt = m_clock->Restart();
	//m_render->Update(m_scene, m_cameraEditor, m_lightManager, m_uniformManager);

	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplVulkan_NewFrame();

	ImGui::NewFrame();

	bool demo = true;
	ImGui::ShowDemoWindow(&demo);

	ImGui::Render();

	m_cameraEditor->Update(m_cameraTransform, dt);

	m_render->Draw(m_scene, m_cameraEditor, m_cameraTransform, m_lightManager, m_uniformManager);
}

void Engine::Cleanup()
{
	delete m_scene;
	m_scene = nullptr;

	delete m_uniformManager;
	m_uniformManager = nullptr;

	m_render->Cleanup();
	delete m_render;
	m_render = nullptr;
}
