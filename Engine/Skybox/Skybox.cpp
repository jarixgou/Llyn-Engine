#include "Skybox.h"

#include <iostream>
#include <stb/stb_image.h>

#include "../Asset/AssetManager.h"
#include "../Memory/MemoryHelper.h"
#include "../Render/Render.h"
#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Utils/Utils.h"
#include "../Vertex/Vertex.h"
#include "../Render/OpenGL/Shader/Shader.h"
#include "../Camera/Camera.h"

namespace Llyn
{
	Skybox::Skybox(const char* _filePath)
	{
		m_vertices =
		{
			{{-1.0f, -1.0f, 1.0f}},	// 0
			{{1.0f, -1.0f, 1.0f}},	// 1
			{{1.0f, 1.0f, 1.0f}},		// 2
			{{-1.0f, 1.0f, 1.0f}},	// 3

			{{1.0f, -1.0f, -1.0f}},	// 4
			{{-1.0f, -1.0f, -1.0f}},	// 5
			{{-1.0f, 1.0f, -1.0f}},	// 6
			{{1.0f, 1.0f, -1.0f}},	// 7

			{{1.0f, -1.0f, 1.0f}},	// 8
			{{1.0f, -1.0f, -1.0f}},	// 9
			{{1.0f, 1.0f, -1.0f}},	// 10
			{{1.0f, 1.0f, 1.0f}},		// 11

			{{-1.0f, -1.0f, -1.0f}},	// 12
			{{-1.0f, -1.0f, 1.0f}},	// 13
			{{-1.0f, 1.0f, 1.0f}},	// 14
			{{-1.0f, 1.0f, -1.0f}},	// 15

			{{-1.0f, 1.0f, 1.0f}},	// 16
			{{1.0f, 1.0f, 1.0f}},		// 17
			{{1.0f, 1.0f, -1.0f}},	// 18
			{{-1.0f, 1.0f, -1.0f}},	// 19

			{{-1.0f, -1.0f, -1.0f}}, // 20
			{{1.0f, -1.0f, -1.0f}},	// 21
			{{1.0f, -1.0f, 1.0f}},	// 22
			{{-1.0f, -1.0f, 1.0f}},	// 23
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

		m_shader = AssetManager::Get()->GetAsset<Shader>("Core/Shaders/Skybox.shader");

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

		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(_filePath, &width, &height, &channels, 0);
		if (!data)
		{
			std::cout << "Can't open the file" << std::endl;
			return;
		}

		int faceSize = width / 4;

		std::vector<unsigned char> right = Utils::ExtractTexturePart(data, channels, { width, height }, { faceSize, faceSize }, { 2 * faceSize , 1 * faceSize });
		std::vector<unsigned char> left = Utils::ExtractTexturePart(data, channels, { width, height }, { faceSize, faceSize }, { 0 * faceSize , 1 * faceSize });
		std::vector<unsigned char> top = Utils::ExtractTexturePart(data, channels, { width, height }, { faceSize, faceSize }, { 1 * faceSize , 0 * faceSize });
		std::vector<unsigned char> bottom = Utils::ExtractTexturePart(data, channels, { width, height }, { faceSize, faceSize }, { 1 * faceSize , 2 * faceSize });
		std::vector<unsigned char> front = Utils::ExtractTexturePart(data, channels, { width, height }, { faceSize, faceSize }, { 1 * faceSize , 1 * faceSize });
		std::vector<unsigned char> back = Utils::ExtractTexturePart(data, channels, { width, height }, { faceSize, faceSize }, { 3 * faceSize , 1 * faceSize });

		GLenum format = channels == 4 ? GL_RGBA : GL_RGB;

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, right.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, left.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, top.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, bottom.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, front.data());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, back.data());

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	Skybox::~Skybox()
	{

	}

	void Skybox::Draw(Camera* _camera)
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		m_shader->Activate();
		_camera->Matrix(m_shader);
		m_shader->SetUniform("skybox", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

		Render::Get()->Draw(m_vao, m_indices);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}
}
