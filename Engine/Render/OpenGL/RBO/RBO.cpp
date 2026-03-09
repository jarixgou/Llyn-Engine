#include "RBO.h"

namespace Llyn
{
	RBO::RBO()
	{
		m_id = 0;
		glGenRenderbuffers(1, &m_id);
	}

	RBO::~RBO()
	{
		glDeleteRenderbuffers(1, &m_id);
	}

	void RBO::Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_id);
	}

	void RBO::Unbind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}