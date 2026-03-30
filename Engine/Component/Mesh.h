#ifndef MESH__H
#define MESH__H

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/fwd.hpp>

#include "Component.h"


namespace Llyn
{
	struct MeshFilter;
	struct Vertex;
	struct Material;
	class Texture;
	class Shader;
	class Camera;

	struct Transform;

	class Mesh : public Component
	{
	private:
		MeshFilter* m_meshFilter;

		Material* m_material;
	public:
		Mesh() = default;
		Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices, Material* _material);
		Mesh(MeshFilter* _filter, Material* _material);
		~Mesh() override = default;

		Mesh(const Mesh&);
		Mesh& operator=(const Mesh&);

		Mesh(Mesh&& _other) = delete;
		Mesh& operator=(Mesh&& _other) = delete;

		void Draw(Camera* _camera, glm::mat4 _model) override;

		Material* GetMaterial();
		void SetMaterial(Material* _material);

		MeshFilter* GetMeshFilter();
		void SetMeshFilter(MeshFilter* _meshFilter);
	};
}

#endif