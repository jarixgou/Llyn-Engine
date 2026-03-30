#include "VAO.h"

#include "../VBO/VBO.h"

namespace Llyn
{
	VAO::VAO()
	{
		glGenVertexArrays(1, &m_id);
	}

	VAO::~VAO()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void VAO::LinkAttrib(VBO* _vbo, GLuint _layout, GLuint _numComponents, GLenum _type, GLsizeiptr _stride, void* _offset)
	{
		_vbo->Bind();

		glVertexAttribPointer(_layout, _numComponents, _type, GL_FALSE, _stride, _offset);
		glEnableVertexAttribArray(_layout);

		_vbo->Unbind();
	}

	void VAO::Bind()
	{
		glBindVertexArray(m_id);
	}

	void VAO::Unbind()
	{
		glBindVertexArray(0);
	}

	VAO& VAO::operator=(const VAO& _other)
	{
		if (this == &_other)
		{
			return *this;
		}

		m_id = _other.m_id;
		return *this;
	}
}
