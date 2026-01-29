#include "Mesh.h"

#include <iostream>

#include "../VAO/VAO.h"
#include "../VBO/VBO.h"
#include "../EBO/EBO.h"
#include "../Vertex/Vertex.h"
#include "../Texture/Texture.h"
#include "../Shader/Shader.h"
#include "../Camera/Camera.h"
#include "../Transform/Transform.h"

namespace ENGINE_NAME
{
	Mesh::Mesh(Mesh&&) noexcept = default;
	Mesh& Mesh::operator=(Mesh&&) noexcept = default;

	Mesh::Mesh(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices, std::vector<Texture>& _textures)
	{
		m_transform = std::make_unique<Transform>();

		m_model = glm::mat4(1.0f);

		m_vertices = _vertices;
		m_indices = _indices;
		m_textures = _textures;

		m_vao = std::make_unique<VAO>();
		m_vao->Bind();

		m_vbo = std::make_unique<VBO>(m_vertices);
		m_ebo = std::make_unique<EBO>(m_indices);
		m_vbo->Bind();
		m_ebo->Bind();

		m_vao->LinkAttrib(m_vbo.get(), 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		m_vao->LinkAttrib(m_vbo.get(), 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		m_vao->LinkAttrib(m_vbo.get(), 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		m_vao->LinkAttrib(m_vbo.get(), 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

		m_vao->Unbind();
		m_vbo->Unbind();
		m_ebo->Unbind();
	}

	Mesh::~Mesh()
	{
		m_vertices.clear();
		m_indices.clear();
		m_textures.clear();
	}

	Transform* Mesh::GetTransform() const
	{
		return m_transform.get();
	}

	void Mesh::Draw
	(
		Shader& _shader,
		Camera& _camera,
		glm::mat4 _matrix,
		glm::vec3 _translation,
		glm::quat _rotation,
		glm::vec3 _scale
	)
	{
		_shader.Activate();
		m_vao->Bind();

		glm::mat3 modelInverse = glm::transpose(glm::inverse(_matrix));

		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (unsigned int i = 0; i < m_textures.size(); ++i)
		{
			std::string num = "";
			std::string type = m_textures[i].GetType();

			if (type == "diffuse")
			{
				num = std::to_string(numDiffuse++);
			}
			else if (type == "specular")
			{
				num = std::to_string(numSpecular++);
			}

			m_textures[i].Bind();
			m_textures[i].TexUnit(_shader, (type + num).c_str(), i);
		}

		_shader.SetUniform("camPos", _camera.GetPosition());
		_camera.Matrix(_shader);

		glm::mat4 trans = glm::mat4(1.f);
		glm::mat4 rot = glm::mat4(1.f);
		glm::mat4 scale = glm::mat4(1.f);

		trans = glm::translate(trans, _translation);
		rot = glm::mat4_cast(_rotation);
		scale = glm::scale(scale, _scale);

		_shader.SetUniform("translation", glm::value_ptr(trans), 1);
		_shader.SetUniform("rotation", glm::value_ptr(rot), 1);
		_shader.SetUniform("scale", glm::value_ptr(scale), 1);
		_shader.SetUniform("model", glm::value_ptr(_matrix), 1);
		_shader.SetUniform("modelInverse", modelInverse);

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
	}
}