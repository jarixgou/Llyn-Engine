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
#include "../Material/Material.h"

#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<GLuint>& _indices, Material* _material)
	{
		m_vertices = _vertices;
		m_indices = _indices;

		if (_material == nullptr)
		{
			AllocateMemory(&m_material);
			AllocateMemory(&m_material->baseMap, "Core/Texture/default.png", 0);
		}
		else
		{
			m_material = _material;
		}

		AllocateMemory(&m_vao);
		m_vao->Bind();

		AllocateMemory(&m_vbo, m_vertices);
		AllocateMemory(&m_ebo, m_indices);
		m_vbo->Bind();
		m_ebo->Bind();

		m_vao->LinkAttrib(m_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		m_vao->LinkAttrib(m_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		m_vao->LinkAttrib(m_vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		m_vao->LinkAttrib(m_vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

		m_vao->Unbind();
		m_vbo->Unbind();
		m_ebo->Unbind();
	}

	Mesh::~Mesh()
	{
		DeleteMemory(&m_vao);
		DeleteMemory(&m_vbo);
		DeleteMemory(&m_ebo);

		DeleteMemory(&m_material);

		m_vertices.clear();
		m_indices.clear();
	}

	Mesh::Mesh(Mesh&& _other) noexcept
	{
		m_vertices = std::move(_other.m_vertices);
		m_indices = std::move(_other.m_indices);

		m_vao = _other.m_vao;
		m_vbo = _other.m_vbo;
		m_ebo = _other.m_ebo;

		m_material = _other.m_material;

		_other.m_vao = nullptr;
		_other.m_vbo = nullptr;
		_other.m_ebo = nullptr;
		_other.m_material = nullptr;
	}

	Mesh& Mesh::operator=(Mesh&& _other) noexcept
	{
		if (this != &_other)
		{
			DeleteMemory(&m_vao);
			DeleteMemory(&m_vbo);
			DeleteMemory(&m_ebo);

			DeleteMemory(&m_material);

			m_vertices = std::move(_other.m_vertices);
			m_indices = std::move(_other.m_indices);

			m_vao = _other.m_vao;
			m_vbo = _other.m_vbo;
			m_ebo = _other.m_ebo;

			m_material = _other.m_material;

			_other.m_vao = nullptr;
			_other.m_vbo = nullptr;
			_other.m_ebo = nullptr;
			_other.m_material = nullptr;
		}
		return *this;
	}

	void Mesh::Draw
	(
		Shader& _shader,
		Camera& _camera,
		glm::mat4 _model
	)
	{
		_shader.Activate();
		m_vao->Bind();

		glm::mat3 modelInverse = glm::transpose(glm::inverse(_model));

		m_material->Bind(_shader);

		_shader.SetUniform("camPos", _camera.GetPosition());
		_camera.Matrix(_shader);

		_shader.SetUniform("model", glm::value_ptr(_model), 1);
		_shader.SetUniform("modelInverse", modelInverse);

		Render::Get()->Draw(m_vao, m_indices, _camera, _shader);
	}
}
