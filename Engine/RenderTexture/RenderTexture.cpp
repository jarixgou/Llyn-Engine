#include "RenderTexture.h"

#include <glm/vec2.hpp>

#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Render/OpenGL/FBO/FBO.h"
#include "../Render/OpenGL/RBO/RBO.h"
#include "../Texture/Texture.h"

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

		ALLOCATE_MEMORY(m_texture, glm::vec2{1920, 1080});
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetId(), 0);


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
