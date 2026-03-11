#ifndef  RBO__H
#define RBO__H

#include <glad/glad.h>
#include <glm/vec2.hpp>

namespace Llyn
{
	class RBO
	{
	private:
		GLuint m_id;
	public:
		RBO(glm::vec2 _size);
		~RBO();

		void Bind();
		void Unbind();

		GLuint GetId() const;
	};
}

#endif