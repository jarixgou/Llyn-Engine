#ifndef EBO__H
#define EBO__H
#include <vector>
#include <glad/glad.h>

namespace Llyn
{
	class EBO
	{
	private:
		GLuint m_id;
	public:
		EBO(std::vector<GLuint>& _indices);
		~EBO();

		void Bind();
		void Unbind();
	};
}

#endif