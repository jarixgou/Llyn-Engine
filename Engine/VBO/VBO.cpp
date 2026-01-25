#include "VBO.h"

namespace Llyn
{
	VBO::VBO(const GLfloat* _vertices, GLsizeiptr _size)
	{
		glGenBuffers(1, &m_id);

		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, _size, _vertices, GL_STATIC_DRAW);
	}

	VBO::~VBO()
	{
		glDeleteBuffers(1, &m_id);
	}

	void VBO::Bind()
	{
		glBindBuffer(1, m_id);
	}

	void VBO::Unbind()
	{
		glBindBuffer(0, m_id);
	}
}
