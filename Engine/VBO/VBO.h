#ifndef VBO__H
#define VBO__H

#include <glad/glad.h>

namespace Llyn
{
	class VBO
	{
	private:
		GLuint m_id;
	public:
		VBO(const GLfloat* _vertices, GLsizeiptr _size);
		~VBO();

		void Bind();
		void Unbind();
	};
}

#endif