#ifndef VAO__H
#define VAO__H

#include <glad/glad.h>

namespace Llyn
{
	class VBO;

	class VAO
	{
	private:
		GLuint m_id;
	public:
		VAO();
		~VAO();

		void LinkAttrib(VBO* _vbo, GLuint _layout, GLuint _numComponents, GLenum _type, GLsizeiptr _stride, void* _offset);
		void Bind();
		void Unbind();
	};
}

#endif