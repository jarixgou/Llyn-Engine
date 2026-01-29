#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera/Camera.h"
#include "Shader/Shader.h"
#include "Shader/FragmentShaderDefault.h"
#include "Shader/VertexShaderDefault.h"

#include "VBO/VBO.h"
#include "VAO/VAO.h"
#include "EBO/EBO.h"
#include "Shader/lightFragmentShader.h"
#include "Shader/lightVertexShader.h"
#include "Texture/Texture.h"
#include "Vertex/Vertex.h"

#include "Mesh/Mesh.h"
#include "Model/Model.h"
#include "Transform/Transform.h"

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

	Llyn::Shader shader(static_cast<const void*>(Llyn::vertexShaderSource), static_cast<const void*>(Llyn::fragmentShaderSource));

	Llyn::Model model("Models/Sword/scene.gltf");

	shader.Activate();
	shader.SetUniform("lightColor", glm::vec4(1.f, 1.f, 1.f, 1.f));
	shader.SetUniform("lightPos", glm::vec3(0.f, 0.f, 0.f));

	Llyn::Camera camera(glm::vec3(0.f, 0.f, 2.f), glm::vec2(1920, 1080.f));

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	
	glfwSwapInterval(0);

	int frameCount = 0;
	double fpsTimerStart = glfwGetTime();
	double currentFPS = 0.0;

	// Main render loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll events
		glfwPollEvents();

		// FPS counting
		frameCount++;
		double now = glfwGetTime();
		double elapsed = now - fpsTimerStart;
		if (elapsed >= 1.0f / 30.f)
		{
			std::string fps = std::to_string((1.0 / elapsed) * frameCount);
			std::string ms = std::to_string((elapsed / frameCount) * 1000);

			std::string final = "Llyn Engine - FPS: " + fps + " / ms: " + ms;

			glfwSetWindowTitle(window, final.c_str());

			// Reset
			frameCount = 0;
			fpsTimerStart = now;
		}

		// Clear the screen with a dark color
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Input(window);
		camera.UpdateMatrix();

		model.Draw(shader, camera);

		glfwSwapBuffers(window);
	}

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
