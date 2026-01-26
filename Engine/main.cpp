#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

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
		// X	  Y		  Z			Color       texCoords
		-0.5f,	-0.5f,	0.0f,	1.f, 0.f, 0.f,	0.f, 0.f,
		-0.5f,	0.5f,	0.0f,	0.f, 1.f, 0.f,	0.f, 1.f,
		0.5f,	0.5f, 	0.0f,	0.f, 0.f, 1.f,	1.f, 0.f,
		0.5f,	-0.5f,	0.0f,	1.f, 1.f, 1.f,	1.f, 1.f,
	};

	GLuint indices[] =
	{
		0, 2, 1,
		0, 3, 2,
	};

	// Create a GLFW window
	GLFWwindow* window = glfwCreateWindow(800, 800, "Llyn Engine", NULL, NULL);
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
	glViewport(0, 0, 800, 800);

	Llyn::Shader shader(static_cast<const void*>(Llyn::vertexShaderSource), static_cast<const void*>(Llyn::fragmentShaderSource));

	Llyn::VAO vao;
	vao.Bind();

	Llyn::VBO vbo(vertices, sizeof(vertices));
	Llyn::EBO ebo(indices, sizeof(indices));

	vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	vao.LinkAttrib(&vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	vao.LinkAttrib(&vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();

	int widthImage;
	int heightImage;
	int numColChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load("texture.png", &widthImage, &heightImage, &numColChannels, 0);

	if (!bytes)
	{
		std::cerr << "Failed to load texture!" << std::endl;
		// Fallback: afficher les couleurs des vertices
	}
	else
	{
		std::cout << "Texture loaded: " << widthImage << "x" << heightImage << " channels: " << numColChannels << std::endl;
	}

	GLuint texture = 0;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint texUni = glGetUniformLocation(shader.getID(), "uTexture");
	shader.Activate();
	glUniform1i(texUni, 0);

	// Main render loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll events
		glfwPollEvents();

		// Clear the screen with a dark color
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Activate();
		glBindTexture(GL_TEXTURE_2D, texture);
		vao.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}

	glDeleteTextures(1, &texture);

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}