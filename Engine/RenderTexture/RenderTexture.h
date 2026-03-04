#ifndef RENDER_TEXTURE__H
#define RENDER_TEXTURE__H

#include <glad/glad.h>
#include <glm/vec4.hpp>



namespace Llyn
{
	class RenderTexture
	{
	private:
		GLuint m_id;
	public:
		RenderTexture(unsigned int& _width, unsigned int& _height);
		~RenderTexture();

		void Bind(const glm::vec4& _clearColor);
		void Unbind();
	};
}

#endif