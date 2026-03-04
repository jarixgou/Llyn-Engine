#include "RBO.h"

namespace Llyn
{
	RBO::RBO(unsigned int& _width, unsigned int& _height)
	{
		m_width = _width;
		m_height = _height;

		glGenRenderbuffers(1, &m_id);
	}

	RBO::~RBO()
	{
		glDeleteRenderbuffers(1, &m_id);
	}

	void RBO::Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_id);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_id);
	}

	void RBO::Unbind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}