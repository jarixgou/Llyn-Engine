#include "RenderTexture.h"

#include "../VAO/VAO.h"
#include "../VBO/VBO.h"

namespace ENGINE_NAME
{
	RenderTexture::RenderTexture(unsigned int& _width, unsigned int& _height)
	{


		VAO vao;
		VBO vbo();

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0,GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0);
	}

	RenderTexture::~RenderTexture()
	{
	}

	void RenderTexture::Bind(const glm::vec4& _clearColor)
	{
	}

	void RenderTexture::Unbind()
	{
	}
}
