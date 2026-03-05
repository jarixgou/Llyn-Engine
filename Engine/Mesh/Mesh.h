#ifndef MESH__H
#define MESH__H

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>



namespace Llyn
{
	class VAO;
	class VBO;
	class EBO;
	struct Vertex;
	class Material;
	class Texture;
	class Shader;
	class Camera;

	struct Transform;

	class Mesh
	{
	private:
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		Material* m_material;

		VAO* m_vao;
		VBO* m_vbo;
		EBO* m_ebo;
	public:
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, Material* _material);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&& _other) noexcept;
		Mesh& operator=(Mesh&& _other) noexcept;

		void Draw
		(
			Shader& _shader,
			Camera& _camera,
			glm::mat4 _model = glm::mat4(1.f)
		);
	};
}

#endif