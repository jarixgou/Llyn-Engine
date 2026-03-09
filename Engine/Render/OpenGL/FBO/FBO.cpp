#include "FBO.h"

namespace Llyn
{
	FBO::FBO()
	{
		m_id = 0;
		glGenFramebuffers(1, &m_id);
	}

	FBO::~FBO()
	{
		glDeleteFramebuffers(1, &m_id);
	}

	void FBO::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	}

	void FBO::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
