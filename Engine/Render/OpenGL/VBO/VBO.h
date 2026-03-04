#ifndef VBO__H
#define VBO__H

#include <vector>
#include <glad/glad.h>

namespace Llyn
{
	struct Vertex;

	class VBO
	{
	private:
		GLuint m_id;
	public:
		VBO(std::vector<Vertex>& _vertices);
		~VBO();

		void Bind();
		void Unbind();
	};
}

#endif