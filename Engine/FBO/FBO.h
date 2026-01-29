#ifndef FBO__H
#define FBO__H

#include <glad/glad.h>

#include "../Macro.h"

namespace ENGINE_NAME
{
	class FBO
	{
	private:
		GLuint m_id;
	public:
		FBO();
		~FBO();

		void Bind();
		void Unbind();
	};
}

#endif