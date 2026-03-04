#ifndef RBO__H
#define RBO__H

#include <glad/glad.h>



namespace Llyn
{
	class RBO
	{
	private:
		GLuint m_id;

		unsigned int m_width;
		unsigned int m_height;
	public:
		RBO(unsigned int& _width, unsigned int& _height);
		~RBO();

		void Bind();
		void Unbind();
	};
}

#endif