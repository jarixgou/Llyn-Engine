#include "RenderTexture.h"
#include "../Vertex/Vertex.h"

#include <iostream>
#include <windows.h>
#include <glm/vec2.hpp>

#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Render/OpenGL/FBO/FBO.h"
#include "../Render/OpenGL/RBO/RBO.h"
#include "../Texture/Texture.h"
#include "../Render/Render.h"
#include "../Render/OpenGL/Shader/Shader.h"

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
			Vertex{{-1.f, -1.f, 0}, {0,0,0}, {0,0,0}, {0,0}},
			Vertex{{-1.f, 1.f, 0}, {0,0,0}, {0,0,0}, {0,1}},
			Vertex{{1.f, 1.f, 0}, {0,0,0}, {0,0,0}, {1,1}},
			Vertex{{1.f, -1.f, 0}, {0,0,0}, {0,0,0}, {1,0}},
		};

		m_indices =
		{
			0, 1, 2, 0, 2, 3
		};
		
		ALLOCATE_MEMORY(m_fbo);
		m_fbo->Bind();

		ALLOCATE_MEMORY(m_texture, glm::vec2{ 1920, 1080 });
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetId(), 0);

		ALLOCATE_MEMORY(m_rbo, glm::vec2(1920, 1080));
		m_rbo->Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo->GetId());

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
		DELETE_MEMORY(m_vao);
		DELETE_MEMORY(m_vbo);
		DELETE_MEMORY(m_ebo);

		DELETE_MEMORY(m_fbo);
		DELETE_MEMORY(m_rbo);

		DELETE_MEMORY(m_texture);

		m_vertices.clear();
		m_indices.clear();
	}

	void RenderTexture::Clear(glm::vec3 _color)
	{
		m_fbo->Bind();

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FBO:: Framebuffer is not complete in Clear()! Status: " << status << std::endl;
		}

		glClearColor(_color.x, _color.y, _color.z, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	}

	void RenderTexture::Display()
	{
		m_fbo->Unbind();
	}

	void RenderTexture::Draw(Shader& _shader)
	{
		GLboolean cullFaceEnable = glIsEnabled(GL_CULL_FACE);

		glDisable(GL_CULL_FACE);

		_shader.Activate();
		m_texture->Bind();
		m_texture->TexUnit(_shader, "screenTexture", 0);

		Render::Get()->Draw(m_vao, m_indices, _shader);

		if (cullFaceEnable)
		{
			glEnable(GL_CULL_FACE);
		}
	}

	void RenderTexture::Bind()
	{
		m_fbo->Bind();
	}

	void RenderTexture::Unbind()
	{
		m_fbo->Unbind();
	}
}
