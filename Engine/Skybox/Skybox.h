#ifndef SKYBOX__H
#define SKYBOX__H
#include <vector>
#include <glad/glad.h>

namespace Llyn
{
	class VAO;
	class VBO;
	class EBO;

	struct Vertex;

	class Shader;
	class Camera;

	class Skybox
	{
	private:
		GLuint m_id;

		Shader* m_shader;

		VAO* m_vao;
		VBO* m_vbo;
		EBO* m_ebo;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
	public:
		Skybox(const char* _filePath);
		~Skybox();

		void Draw(Camera* _camera);
	};
}

#endif