#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader/Shader.h"
#include "Shader/FragmentShaderDefault.h"
#include "Shader/VertexShaderDefault.h"

#include "VBO/VBO.h"
#include "VAO/VAO.h"
#include "EBO/EBO.h"

int main()
{
	// Initialize GLFW
	glfwInit();

	// Configure GLFW for OpenGL 4.6 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[] =
	{
		// X,					Y						Z			Color
		-0.5f,		-0.5f * std::sqrt(3.f) / 3,			0.0f,	0.8f, 0.3f, 0.02f,	// Lower left corner
		0.5f,		-0.5f * std::sqrt(3.f) / 3,			0.0f,	0.8f, 0.3f, 0.02f,	// Lower right corner
		0.f,		0.5f * std::sqrt(3.f) * 2 / 3,		0.0f,	1.0f, 0.6f, 0.32f,	// Top corner
		-0.5f / 2,	0.5f * std::sqrt(3.f) / 6,			0.0f,	0.9f, 0.45f, 0.17f,	// Middle left
		0.5f / 2,	0.5f * std::sqrt(3.f) / 6,			0.0f,	0.9f, 0.45f, 0.17f,	// Middle right
		0.f,		-0.5f * std::sqrt(3.f) / 3,			0.0f,	0.8f, 0.3f, 0.02f	// Bottom
	};

	GLuint indices[] =
	{
		0, 3, 5,
		3, 2, 4,
		5, 4, 1
	};

	// Create a GLFW window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Llyn Engine", NULL, NULL);
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
	glViewport(0, 0, 800, 600);

	Llyn::Shader shader(static_cast<const void*>(Llyn::vertexShaderSource), static_cast<const void*>(Llyn::fragmentShaderSource));

	Llyn::VAO vao;
	vao.Bind();

	Llyn::VBO vbo(vertices, sizeof(vertices));
	Llyn::EBO ebo(indices, sizeof(indices));

	vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	vao.LinkAttrib(&vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();

	// Main render loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll events
		glfwPollEvents();

		// Clear the screen with a dark color
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Activate();
		vao.Bind();
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}