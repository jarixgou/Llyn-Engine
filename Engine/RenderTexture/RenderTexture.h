#ifndef RENDER_TEXTURE__H
#define RENDER_TEXTURE__H
#include <vector>
#include <glad/glad.h>
#include <glm/vec3.hpp>

namespace Llyn
{
	class VAO;
	class VBO;
	class EBO;

	class FBO;
	class RBO;

	class Texture;
	class Shader;

	struct Vertex;

	class RenderTexture
	{
	private:
		VAO* m_vao;
		VBO* m_vbo;
		EBO* m_ebo;

		FBO* m_fbo;
		RBO* m_rbo;

		Texture* m_texture;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;

	public:
		RenderTexture();
		~RenderTexture();

		void Clear(glm::vec3 _color);
		void Display();

		void Draw(Shader& _shader);

		void Bind();
		void Unbind();
	};
}

#endif