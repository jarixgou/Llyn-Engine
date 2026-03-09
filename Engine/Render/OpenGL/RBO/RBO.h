#ifndef  RBO__H
#define RBO__H

#include <glad/glad.h>

namespace Llyn
{
	class RBO
	{
	private:
		GLuint m_id;
	public:
		RBO();
		~RBO();

		void Bind();
		void Unbind();
	};
}

#endif