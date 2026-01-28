#ifndef MESH__H
#define MESH__H

#include <vector>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "../Macro.h"

namespace ENGINE_NAME
{
	class VAO;
	class VBO;
	class EBO;
	struct Vertex;
	class Texture;
	class Shader;
	class Camera;

	struct Transform;

	class Mesh
	{
	private:
		Transform* m_transform;

		glm::mat4 m_model;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		std::vector<Texture> m_textures;

		VAO* m_vao;
		VBO* m_vbo;
		EBO* m_ebo;
	public:
		Mesh(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices, std::vector<Texture>& _textures);
		~Mesh();

		Transform* GetTransform() const;

		void Draw(Shader& _shader, Camera& _camera);
	};
}

#endif