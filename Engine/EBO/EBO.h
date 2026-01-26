#ifndef EBO__H
#define EBO__H
#include <glad/glad.h>
#include "../Macro.h"

namespace ENGINE_NAME
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