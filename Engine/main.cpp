#include <iostream>
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
#include "Transform/Transform.h"

int main()
{
	// Initialize GLFW
	glfwInit();

	// Configure GLFW for OpenGL 4.6 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Vertices coordinates
	Llyn::Vertex vertices[] =
	{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
		Llyn::Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
		Llyn::Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
		Llyn::Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		Llyn::Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	Llyn::Vertex lightVertices[]
	{ //     COORDINATES     //
		Llyn::Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
		Llyn::Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
		Llyn::Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
		Llyn::Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
		Llyn::Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
		Llyn::Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
		Llyn::Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
		Llyn::Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
	};

	GLuint lightIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};

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

	Llyn::Texture texture[] =
	{
		Llyn::Texture("planks.png", "diffuse", 0),
		Llyn::Texture("planksSpec.png", "specular", 1),
	};

	Llyn::Shader shader(static_cast<const void*>(Llyn::vertexShaderSource), static_cast<const void*>(Llyn::fragmentShaderSource));
	std::vector<Llyn::Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Llyn::Vertex));
	std::vector<GLuint> inds(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector<Llyn::Texture> texs(texture, texture + sizeof(texture) / sizeof(Llyn::Texture));
	Llyn::Mesh plane(verts, inds, texs);

	Llyn::Shader lightShader(static_cast<const void*>(Llyn::lightVertexShader), static_cast<const void*>(Llyn::lightFragmentShader));
	std::vector<Llyn::Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Llyn::Vertex));
	std::vector<GLuint> lightInds(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	Llyn::Mesh light(lightVerts, lightInds, texs);
	auto lightTransform = light.GetTransform();
	lightTransform->position = glm::vec3(0.5f, 0.5f, 0.5f);

	glm::vec4 lightColor = glm::vec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	lightShader.Activate();
	lightShader.SetUniform("lightColor", lightColor);

	shader.Activate();
	shader.SetUniform("lightColor", lightColor);
	shader.SetUniform("lightPos", lightPos);

	float rotationX = 0.f;
	float rotationY = 0.f;
	float rotationZ = 0.f;
	double prevTime = glfwGetTime();

	Llyn::Camera camera(glm::vec3(0.f, 0.f, 2.f), glm::vec2(1920, 1080.f));

	glEnable(GL_DEPTH_TEST);

	// Main render loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll events
		glfwPollEvents();

		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60.f)
		{
			rotationX += 1.8f;
			rotationY += 1.4f;
			rotationZ += 1.6f;

			float posX = 0.5f * sin(glfwGetTime());
			float posZ = 0.5f * cos(glfwGetTime());
			float posY = 1.5f * abs(0.5f + 0.5f * sin(glfwGetTime() * 0.5f));

			lightTransform->position = glm::vec3(posX, posY, posZ);
			lightPos = lightTransform->position;
			lightTransform->rotation = glm::vec3(rotationX, rotationY, rotationZ);

			shader.Activate();
			shader.SetUniform("lightPos", lightPos);

			prevTime = crntTime;
		}

		// Clear the screen with a dark color
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Input(window);
		camera.UpdateMatrix();

		plane.Draw(shader, camera);

		light.Draw(lightShader, camera);

		glfwSwapBuffers(window);
	}

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
