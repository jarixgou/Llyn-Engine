#include "Mesh.h"

#include <iostream>

#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Vertex/Vertex.h"
#include "../Texture/Texture.h"
#include "../Render/OpenGL/Shader/Shader.h"
#include "../Camera/Camera.h"
#include "../Render/Render.h"
#include "../Asset/Material.h"

#include "../Memory/MemoryHelper.h"

#include "../Asset/AssetManager.h"
#include "../Mesh/MeshFilter.h"

namespace Llyn
{
	Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<uint32_t>& _indices, Material* _material)
	{
		ALLOCATE_MEMORY(m_meshFilter);
		m_meshFilter->vertices = _vertices;
		m_meshFilter->indices = _indices;

		if (_material == nullptr)
		{
			m_material = AssetManager::Get()->GetAsset<Material>("Core/Material/Default.mat");
		}
		else
		{
			m_material = _material;
		}

		ALLOCATE_MEMORY(m_meshFilter->vao);
		if (m_meshFilter->vao != nullptr)
		{
			m_meshFilter->vao->Bind();
		}

		ALLOCATE_MEMORY(m_meshFilter->vbo, m_meshFilter->vertices);
		ALLOCATE_MEMORY(m_meshFilter->ebo, m_meshFilter->indices);
		if (m_meshFilter->vao != nullptr && m_meshFilter->vbo != nullptr && m_meshFilter->ebo != nullptr)
		{
			m_meshFilter->vbo->Bind();
			m_meshFilter->ebo->Bind();

			m_meshFilter->vao->LinkAttrib(m_meshFilter->vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
			m_meshFilter->vao->LinkAttrib(m_meshFilter->vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
			m_meshFilter->vao->LinkAttrib(m_meshFilter->vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
			m_meshFilter->vao->LinkAttrib(m_meshFilter->vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

			m_meshFilter->vao->Unbind();
			m_meshFilter->vbo->Unbind();
			m_meshFilter->ebo->Unbind();
		}
	}

	Mesh::Mesh(MeshFilter* _filter, Material* _material)
	{
		m_meshFilter = _filter;

		if (_material == nullptr)
		{
			m_material = AssetManager::Get()->GetAsset<Material>("Core/Material/Default.mat");
		}
		else
		{
			m_material = _material;
		}
	}

	Mesh::Mesh(const Mesh& _other)
		: Component(_other)
	{
		m_meshFilter = _other.m_meshFilter;

		m_material = _other.m_material;
	}

	Mesh& Mesh::operator=(const Mesh& _other)
	{
		if (this == &_other)
		{
			return *this;
		}

		m_meshFilter = _other.m_meshFilter;

		m_material = _other.m_material;

		return *this;
	}

	void Mesh::Draw(Camera* _camera, glm::mat4 _model)
	{
		if (m_material != nullptr)
		{
			m_material->Bind(_camera, _model);
		}

		Render::Get()->Draw(m_meshFilter->vao, m_meshFilter->indices);
	}

	Material* Mesh::GetMaterial()
	{
		return m_material;
	}

	void Mesh::SetMaterial(Material* _material)
	{
		m_material = _material;
	}

	MeshFilter* Mesh::GetMeshFilter()
	{
		return m_meshFilter;
	}

	void Mesh::SetMeshFilter(MeshFilter* _meshFilter)
	{
		m_meshFilter = _meshFilter;
	}
}
