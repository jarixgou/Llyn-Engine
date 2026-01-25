#ifndef EBO__H
#define EBO__H
#include <glad/glad.h>

namespace Llyn
{
	class EBO
	{
	private:
		GLuint m_id;
	public:
		EBO(const GLuint* _indices, GLsizeiptr _size);
		~EBO();

		void Bind();
		void Unbind();
	};
}

#endif