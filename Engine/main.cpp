#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Asset/AssetManager.h"
#include "External/imgui/imgui_impl_glfw.h"
#include "External/imgui/imgui_impl_opengl3.h"

#include "Camera/Camera.h"
#include "Clock/Clock.h"
#include "External/imgui/imgui_internal.h"
#include "Frontend/Hierarchy.h"
#include "Frontend/Inspector.h"
#include "GameObject/GameObject.h"
#include "Render/OpenGL/Shader/Shader.h"

#include "Component//Mesh.h"
#include "Grid/Grid.h"
#include "Mesh/MeshHelper.h"
#include "Model/Model.h"
#include "Render/Render.h"
#include "RenderTexture/RenderTexture.h"
#include "Skybox/Skybox.h"
#include "Vertex/Vertex.h"

#define DRAW_IMGUI_DEMO false

void DrawHierachy(std::vector<Llyn::GameObject*>& _goList);
void DrawTreeHierachy(std::vector<Llyn::GameObject*>& _goList);

int main()
{
	// Initialize GLFW
	glfwInit();

	// Configure GLFW for OpenGL 4.6 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFW window
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Llyn Engine", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL functions using GLAD
	gladLoadGL();

	// Load ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// Set the viewport
	glViewport(0, 0, 1920, 1080);

	Llyn::Shader* shader = Llyn::AssetManager::Get()->GetAsset<Llyn::Shader>("Core/Shaders/Default.shader");
	Llyn::Shader* shaderGrid = Llyn::AssetManager::Get()->GetAsset<Llyn::Shader>("Core/Shaders/EndlessGrid.shader");
	Llyn::Shader* renderTextureShader = Llyn::AssetManager::Get()->GetAsset<Llyn::Shader>("Core/Shaders/RenderTexture.shader");
	Llyn::Shader* skyboxShader = Llyn::AssetManager::Get()->GetAsset<Llyn::Shader>("Core/Shaders/Skybox.shader");

	shader->Activate();
	shader->SetUniform("light.type", 1);
	shader->SetUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader->SetUniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader->SetUniform("light.specular", glm::vec3(1.f, 1.f, 1.f));
	shader->SetUniform("light.position", glm::vec3(0.f, 5.f, 0.f));

	shader->SetUniform("light.direction", glm::vec3(0.f, -1.f, 0.f));

	shader->SetUniform("light.constant", 1.f);
	shader->SetUniform("light.linear", 0.09f);
	shader->SetUniform("light.quadratic", 0.032f);

	Llyn::Camera camera(glm::vec3(0.f, 0.f, 2.f), glm::vec2(1920, 1080.f));

	Llyn::Model model("Models/Backpack/scene.gltf");

	Llyn::Grid grid;
	Llyn::RenderTexture renderTexture = Llyn::RenderTexture();
	Llyn::Skybox skybox("Skybox.png");

	glfwSwapInterval(0);

	std::vector<Llyn::GameObject*> goList;
	goList.reserve(2);
	for (int i = 0; i < 2; ++i)
	{
		goList.emplace_back(new Llyn::GameObject());
	}

	Llyn::Hierarchy::Get()->SetGameObjectList(&goList);

	Llyn::Clock clock;

	// Main render loop
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	float fpsDt = 0.f;
	float timerFps = 0.f;

	while (!glfwWindowShouldClose(window))
	{
		// Poll events
		glfwPollEvents();

		float dt = clock.Restart();
		timerFps += dt;
		if (timerFps >= 0.2f)
		{
			fpsDt = dt;
			timerFps = 0.f;
		}

		camera.Input(window, dt);
		camera.Update();

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		skybox.Draw(&camera);

		glDisable(GL_CULL_FACE);

		grid.Draw(&camera);

		glEnable(GL_CULL_FACE);

		for (auto & go : goList)
		{
			go->Draw(&camera);
		}

		bool demo = DRAW_IMGUI_DEMO;
		ImGui::ShowDemoWindow(&demo);

		ImGui::Begin("Info");
		ImGui::Text("FPS : %f", 1 / fpsDt);
		ImGui::End();

		Llyn::Hierarchy::Get()->Draw();
		Llyn::Inspector::Get()->Draw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backUpCurrentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backUpCurrentContext);
		}

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}