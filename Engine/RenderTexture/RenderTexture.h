#ifndef RENDER_TEXTURE__H
#define RENDER_TEXTURE__H

namespace Llyn
{
	class VAO;
	class VBO;
	class EBO;

	class FBO;
	class RBO;

	class Texture;

	class RenderTexture
	{
	private:
		VAO* m_vao;
		VBO* m_vbo;
		EBO* m_ebo;

		FBO* m_fbo;
		RBO* m_rbo;

		Texture* m_texture;
	public:
		RenderTexture();
		~RenderTexture();

		void Bind();
		void Unbind();
	};
}

#endif