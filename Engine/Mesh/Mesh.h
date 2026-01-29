#ifndef MESH__H
#define MESH__H

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

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
		std::unique_ptr<Transform> m_transform;

		glm::mat4 m_model;

		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		std::vector<Texture> m_textures;

		std::unique_ptr<VAO> m_vao;
		std::unique_ptr<VBO> m_vbo;
		std::unique_ptr<EBO> m_ebo;
	public:
		Mesh(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices, std::vector<Texture>& _textures);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&&) noexcept;
		Mesh& operator=(Mesh&&) noexcept;

		Transform* GetTransform() const;

		void Draw
		(
			Shader& _shader,
			Camera& _camera,
			glm::mat4 _matrix = glm::mat4(1.f),
			glm::vec3 _translation = glm::vec3(0.f, 0.f, 0.f),
			glm::quat _rotation = glm::quat(1.f, 0.f, 0.f, 0.f),
			glm::vec3 _scale = glm::vec3(1.f, 1.f, 1.f)
		);
	};
}

#endif