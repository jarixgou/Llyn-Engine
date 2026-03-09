#ifndef FBO__H
#define FBO__H
#include <glad/glad.h>

namespace Llyn
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
