#include "Render.h"

#include <iostream>
#include <vector>
#include <glad/glad.h>

#include "glm/vec4.hpp"
#include "OpenGL/VAO/VAO.h"
#include "OpenGL/Shader/Shader.h"


namespace Llyn
{
	void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}

	bool GLLogCall(const char* _function, const char* _file, int _line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "[OpenGL Error] (" << error << ") : " << _function << " " << _file << ":" << _line << std::endl;
			return false;
		}
		return true;
	}

	Render* Render::s_instance = nullptr;

	Render::Render()
	{
		m_renderMode = RenderMode::TRIANGLES;
	}

	Render* Render::Get()
	{
		if (s_instance == nullptr)
		{
			s_instance = new Render();
		}

		return s_instance;
	}

	void Render::SetRenderMode(RenderMode _renderMode)
	{
		m_renderMode = _renderMode;
	}

	RenderMode Render::GetRenderMode()
	{
		return m_renderMode;
	}

	void Render::Clear(glm::vec4 _color)
	{
		glClearColor(_color.r, _color.g, _color.b, _color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Render::Draw(VAO* _vao, std::vector<GLuint>& _indices, Camera& _camera, Shader& _shader) const
	{
		_shader.Activate();
		_vao->Bind();

		if (m_renderMode == RenderMode::TRIANGLES)
		{
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);
		}
		else if (m_renderMode == RenderMode::LINE)
		{
			glDrawElements(GL_LINE_LOOP, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);
		}
		else if (m_renderMode == RenderMode::TRIANGLES_AND_LINE)
		{
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);
		}
	}
}
