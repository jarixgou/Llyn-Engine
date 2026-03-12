#include "Skybox.h"

#include <stb/stb_image.h>

#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Vertex/Vertex.h"

namespace Llyn
{
	Skybox::Skybox(const char* _filePath)
	{
		m_vertices = 
		{
			{{-0.1f, -0.1f, 0.1f}},	// 0
			{{0.1f, -0.1f, 0.1f}},	// 1
			{{0.1f, 0.1f, 0.1f}},		// 2
			{{-0.1f, 0.1f, 0.1f}},	// 3

			{{0.1f, -0.1f, -0.1f}},	// 4
			{{-0.1f, -0.1f, -0.1f}},	// 5
			{{-0.1f, 0.1f, -0.1f}},	// 6
			{{0.1f, 0.1f, -0.1f}},	// 7

			{{0.1f, -0.1f, 0.1f}},	// 8
			{{0.1f, -0.1f, -0.1f}},	// 9
			{{0.1f, 0.1f, -0.1f}},	// 10
			{{0.1f, 0.1f, 0.1f}},		// 11

			{{-0.1f, -0.1f, -0.1f}},	// 12
			{{-0.1f, -0.1f, 0.1f}},	// 13
			{{-0.1f, 0.1f, 0.1f}},	// 14
			{{-0.1f, 0.1f, -0.1f}},	// 15

			{{-0.1f, 0.1f, 0.1f}},	// 16
			{{0.1f, 0.1f, 0.1f}},		// 17
			{{0.1f, 0.1f, -0.1f}},	// 18
			{{-0.1f, 0.1f, -0.1f}},	// 19

			{{-0.1f, -0.1f, -0.1f}}, // 20
			{{0.1f, -0.1f, -0.1f}},	// 21
			{{0.1f, -0.1f, 0.1f}},	// 22
			{{-0.1f, -0.1f, 0.1f}},	// 23
		};

		m_indices =
		{
			0, 2, 1,	0, 3, 2,

			4, 6, 5,	4, 7, 6,

			8, 10, 9,	8, 11, 10,

			12, 14, 13,	12, 15, 14,

			16, 18, 17,	16, 19, 18,

			20, 22, 21,	20, 23, 22
		};

		m_id = 0;

		m_vao = nullptr;
		m_vbo = nullptr;
		m_ebo = nullptr;

		ALLOCATE_MEMORY(m_vao);
		m_vao->Bind();

		ALLOCATE_MEMORY(m_vbo, m_vertices);
		ALLOCATE_MEMORY(m_ebo, m_indices);

		m_vbo->Bind();
		m_ebo->Bind();

		m_vao->LinkAttrib(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);

		m_vao->Unbind();
		m_vbo->Unbind();
		m_ebo->Unbind();

		int width = 0;
		int height = 0;
		int channels = 0;

		unsigned char* data = stbi_load(_filePath, &width, &height, &channels, 0);

		if (!data)
		{
			throw errno("Can't open the file");
		}

		int faceSize = width / 4;


	}

	Skybox::~Skybox()
	{

	}

	void Skybox::Draw(Shader& _shader, Camera& _camera)
	{
	}
}
