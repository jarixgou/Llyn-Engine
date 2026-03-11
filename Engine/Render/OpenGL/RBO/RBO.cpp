#include "RBO.h"

#include <functional>

namespace Llyn
{
	RBO::RBO(glm::vec2 _size)
	{
		m_id = 0;
		glGenRenderbuffers(1, &m_id);
		Bind();
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x, _size.y);
		Unbind();
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

	GLuint RBO::GetId() const
	{
		return m_id;
	}
}
