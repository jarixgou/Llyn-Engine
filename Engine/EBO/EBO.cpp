#include "EBO.h"

namespace ENGINE_NAME
{
	EBO::EBO(const GLuint* _indices, GLsizeiptr _size)
	{
		glGenBuffers(1, &m_id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _indices, GL_STATIC_DRAW);
	}

	EBO::~EBO()
	{
		glDeleteBuffers(1, &m_id);
	}

	void EBO::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void EBO::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
