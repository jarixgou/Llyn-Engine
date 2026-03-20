#ifndef MESH__H
#define MESH__H

#include <vector>
#include <glad/glad.h>
#include <glm/fwd.hpp>

#include "../Component/IComponent.h"


namespace Llyn
{
	class VAO;
	class VBO;
	class EBO;
	struct Vertex;
	struct Material;
	class Texture;
	class Shader;
	class Camera;

	struct Transform;

	class Mesh : public IComponent
	{
	private:
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		Material* m_material;

		VAO* m_vao;
		VBO* m_vbo;
		EBO* m_ebo;
	public:
		Mesh() = default;
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices, Material* _material);
		~Mesh() override;

		Mesh(const Mesh&);
		Mesh& operator=(const Mesh&);

		Mesh(Mesh&& _other) noexcept;
		Mesh& operator=(Mesh&& _other) noexcept;

		void Draw(Camera* _camera, glm::mat4 _model) override;

		Material* GetMaterial();
		void SetMaterial(Material* _material);
	};
}

#endif