#include "RenderTexture.h"

#include <iostream>
#include <glm/vec2.hpp>

#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Render/OpenGL/FBO/FBO.h"
#include "../Render/OpenGL/RBO/RBO.h"
#include "../Texture/Texture.h"
#include "../Vertex/Vertex.h"

namespace Llyn
{
	RenderTexture::RenderTexture()
	{
		m_vao = nullptr;
		m_vbo = nullptr;
		m_ebo = nullptr;

		m_fbo = nullptr;
		m_rbo = nullptr;

		m_texture = nullptr;

		m_vertices =
		{
			Vertex{{0.f, 0.f, 0}, {0,0,0}, {0,0,0}, {0,0}},
			Vertex{{0.f, 1.f, 0}, {0,0,0}, {0,0,0}, {0,1}},
			Vertex{{1.f, 1.f, 0}, {0,0,0}, {0,0,0}, {1,1}},
			Vertex{{1.f, 0.f, 0}, {0,0,0}, {0,0,0}, {1,0}},
		};

		m_indices =
		{
			0, 1, 3, 1, 2, 3
		};

		ALLOCATE_MEMORY(m_fbo);
		m_fbo->Bind();

		ALLOCATE_MEMORY(m_texture, glm::vec2{ 1920, 1080 });
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetId(), 0);

		ALLOCATE_MEMORY(m_rbo, glm::vec2(1920, 1080));
		m_rbo->Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo->GetId());

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;
		}

		ALLOCATE_MEMORY(m_vao);
		m_vao->Bind();

		ALLOCATE_MEMORY(m_vbo, m_vertices);
		ALLOCATE_MEMORY(m_ebo, m_indices);

		m_vbo->Bind();
		m_ebo->Bind();

		m_vao->LinkAttrib(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
		m_vao->LinkAttrib(m_vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

		m_vao->Unbind();
		m_ebo->Unbind();
		m_vbo->Unbind();

		m_fbo->Unbind();
		m_rbo->Unbind();
	}

	RenderTexture::~RenderTexture()
	{

	}

	void RenderTexture::Bind()
	{

	}

	void RenderTexture::Unbind()
	{

	}
}
