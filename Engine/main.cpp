#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera/Camera.h"
#include "Render/OpenGL/Shader/Shader.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshHelper.h"
#include "Model/Model.h"
#include "Render/Render.h"
#include "RenderTexture/RenderTexture.h"
#include "Skybox/Skybox.h"
#include "Transform/Transform.h"
#include "Vertex/Vertex.h"

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

	// Set the viewport
	glViewport(0, 0, 1920, 1080);

	Llyn::Shader shader("Core/Shader/default.vert", "Core/Shader/default.frag");
	Llyn::Shader shaderGrid("Core/Shader/EndlessGrid.vert", "Core/Shader/EndlessGrid.frag");
	Llyn::Shader renderTextureShader("Core/Shader/RenderTexture.vert", "Core/Shader/RenderTexture.frag");
	Llyn::Shader skyboxShader("Core/Shader/Skybox.vert", "Core/Shader/Skybox.frag");

	shader.Activate();
	shader.SetUniform("light.type", 1);
	shader.SetUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.SetUniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.SetUniform("light.specular", glm::vec3(1.f, 1.f, 1.f));
	shader.SetUniform("light.position", glm::vec3(0.f, 5.f, 0.f));

	shader.SetUniform("light.direction", glm::vec3(0.f, -1.f, 0.f));

	shader.SetUniform("light.constant", 1.f);
	shader.SetUniform("light.linear", 0.09f);
	shader.SetUniform("light.quadratic", 0.032f);

	Llyn::Camera camera(glm::vec3(0.f, 0.f, 2.f), glm::vec2(1920, 1080.f));

	Llyn::Model model("Models/Backpack/scene.gltf");

	Llyn::Mesh plane(Llyn::CreatePlane());
	Llyn::RenderTexture renderTexture = Llyn::RenderTexture();

	Llyn::Skybox skybox("Skybox.png");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glfwSwapInterval(0);

	double lasteFrame = 0.0f;
	float dt = 0.0f;

	// Main render loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll events
		glfwPollEvents();

		// FPS counting
		double currentFrame = glfwGetTime();
		dt = currentFrame - lasteFrame;
		lasteFrame = currentFrame;

		std::string fps = "FPS : " + std::to_string(1.0f / dt);
		std::string ms = std::to_string(dt * 1000) + "ms";

		glfwSetWindowTitle(window, std::string(fps + " / " + ms).c_str());

		camera.Input(window, dt);
		camera.UpdateMatrix();

		//cube.Draw(shader, camera);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skyboxShader.Activate();
		camera.Matrix(skyboxShader);
		skybox.Draw(skyboxShader, camera);

		glDisable(GL_CULL_FACE);

		shaderGrid.Activate();
		camera.Matrix(shaderGrid);
		shaderGrid.SetUniform("uCamPos", camera.GetPosition());
		plane.Draw(shaderGrid, camera);

		glEnable(GL_CULL_FACE);

		model.Draw(shader, camera);

		glfwSwapBuffers(window);
	}

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
