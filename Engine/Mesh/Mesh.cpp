#include "Mesh.h"

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
	Mesh::Mesh(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices, std::vector<Texture>& _textures)
	{
		m_transform = new Transform();

		m_model = glm::mat4(1.0f);

		m_vertices = _vertices;
		m_indices = _indices;
		m_textures = _textures;

		m_vao = new VAO();
		m_vao->Bind();

		m_vbo = new VBO(m_vertices);
		m_ebo = new EBO(m_indices);

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
		m_vertices.clear();
		m_indices.clear();
		m_textures.clear();
		delete m_transform;
		delete m_vao;
		delete m_vbo;
		delete m_ebo;
	}

	Transform* Mesh::GetTransform() const
	{
		return m_transform;
	}

	void Mesh::Draw(Shader& _shader, Camera& _camera)
	{
		_shader.Activate();
		m_vao->Bind();

		// Reset model matrix
		m_model = glm::mat4(1.0f);

		// Apply position
		m_model = glm::translate(m_model, m_transform->position);

		// Apply rotation
		m_model = glm::rotate(m_model, glm::radians(m_transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_model = glm::rotate(m_model, glm::radians(m_transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_model = glm::rotate(m_model, glm::radians(m_transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Apply scale
		m_model = glm::scale(m_model, m_transform->scale);
		glm::mat3 modelInverse = glm::transpose(glm::inverse(m_model));

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
			m_textures[i].TexUnit(_shader, (type + num).c_str(), i);
			m_textures[i].Bind();
		}
		_shader.SetUniform("model", glm::value_ptr(m_model), 1);
		_shader.SetUniform("modelInverse", modelInverse);
		_shader.SetUniform("camPos", _camera.GetPosition());
		_camera.Matrix(_shader);

		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}
