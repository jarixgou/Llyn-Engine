#include "Shader.h"

#include "../../../Vector/Vector.h"
#include "../../../Utils/Utils.h"

#include <ios>
#include <cerrno>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <json/json.hpp>

#include "../../Render.h"

namespace Llyn
{
	Shader::Shader(const char* _vertexShader, const char* _fragmentShader)
	{
		m_id = glCreateProgram();
		CompileShader(Utils::GetFileContents(_vertexShader).c_str(), GL_VERTEX_SHADER);
		CompileShader(Utils::GetFileContents(_fragmentShader).c_str(), GL_FRAGMENT_SHADER);
	}

	Shader::Shader(const void* _vertexShader, const void* _fragmentShader)
	{
		m_id = glCreateProgram();
		CompileShader(static_cast<const char*>(_vertexShader), GL_VERTEX_SHADER);
		CompileShader(static_cast<const char*>(_fragmentShader), GL_FRAGMENT_SHADER);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_id);
	}

	bool Shader::Load(const char* _path)
	{
		m_id = 0;

		if (!Utils::CheckFileExtension(_path, "shader"))
		{
			return false;
		}

		std::ifstream file(_path);

		if (!file.is_open())
		{
			return false;
		}

		nlohmann::json data = nlohmann::json::parse(file);

		file.close();

		std::string vertexData;
		std::string fragmentData;
		if (data.contains("Vertex") && data["Vertex"].is_string())
		{
			vertexData = data["Vertex"].get<std::string>();
		}
		if (data.contains("Fragment") && data["Fragment"].is_string())
		{
			fragmentData = data["Fragment"].get<std::string>();
		}

		m_id = glCreateProgram();
		CompileShader(Utils::GetFileContents(vertexData.c_str()).c_str(), GL_VERTEX_SHADER);
		CompileShader(Utils::GetFileContents(fragmentData.c_str()).c_str(), GL_FRAGMENT_SHADER);

		return true;
	}

	bool Shader::Save()
	{
		return false;
	}
		
	void Shader::Activate() const
	{
		int currentProgram = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		if (currentProgram != m_id)
		{
			glUseProgram(m_id);
		}
	}

	const GLuint& Shader::getID() const
	{
		return m_id;
	}

	void Shader::SetUniform(const char* _name, const GLuint& _value)
	{
		GL_CALL(glUniform1ui(GetUniformLocation(_name), _value))
	}

	void Shader::SetUniform(const char* _name, const GLint& _value)
	{
		GL_CALL(glUniform1i(GetUniformLocation(_name), _value))
	}

	void Shader::SetUniform(const char* _name, const GLfloat& _value)
	{
		GL_CALL(glUniform1f(GetUniformLocation(_name), _value))
	}

	void Shader::SetUniform(const char* _name, const Vector2u& _value)
	{
		GL_CALL(glUniform2ui(GetUniformLocation(_name), _value.x, _value.y))
	}

	void Shader::SetUniform(const char* _name, const Vector2i& _value)
	{
		GL_CALL(glUniform2i(GetUniformLocation(_name), _value.x, _value.y))
	}

	void Shader::SetUniform(const char* _name, const Vector2f& _value)
	{
		GL_CALL(glUniform2f(GetUniformLocation(_name), _value.x, _value.y))
	}

	void Shader::SetUniform(const char* _name, const Vector3u& _value)
	{
		GL_CALL(glUniform3ui(GetUniformLocation(_name), _value.x, _value.y, _value.z))
	}

	void Shader::SetUniform(const char* _name, const Vector3i& _value)
	{
		GL_CALL(glUniform3i(GetUniformLocation(_name), _value.x, _value.y, _value.z))
	}

	void Shader::SetUniform(const char* _name, const glm::vec3& _value)
	{
		GL_CALL(glUniform3f(GetUniformLocation(_name), _value.x, _value.y, _value.z))
	}

	void Shader::SetUniform(const char* _name, const Vector4u& _value)
	{
		GL_CALL(glUniform4ui(GetUniformLocation(_name), _value.x, _value.y, _value.z, _value.w))
	}

	void Shader::SetUniform(const char* _name, const Vector4i& _value)
	{
		GL_CALL(glUniform4i(GetUniformLocation(_name), _value.x, _value.y, _value.z, _value.w))
	}

	void Shader::SetUniform(const char* _name, const glm::vec4& _value)
	{
		GL_CALL(glUniform4f(GetUniformLocation(_name), _value.x, _value.y, _value.z, _value.w))
	}

	void Shader::SetUniform(const char* _name, const GLfloat* _matrix4fv, int _size)
	{
		GL_CALL(glUniformMatrix4fv(GetUniformLocation(_name), _size, GL_FALSE, _matrix4fv))
	}

	void Shader::SetUniform(const char* _name, const glm::mat3& _value)
	{
		GL_CALL(glUniformMatrix3fv(GetUniformLocation(_name), 1, GL_FALSE, glm::value_ptr(_value)))
	}

	void Shader::CompileShader(const char* _source, GLenum _type) const
	{
		GLuint shader = glCreateShader(_type);

		glShaderSource(shader, 1, &_source, NULL);
		glCompileShader(shader);

		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "Error compiling shader: " << infoLog << std::endl;
		}

		glAttachShader(m_id, shader);
		glLinkProgram(m_id);
		glDeleteShader(shader);

	}

	int Shader::GetUniformLocation(const char* _name)
	{
		if (m_uniformLocationCache.find(_name) != m_uniformLocationCache.end())
		{
			return m_uniformLocationCache[_name];
		}

		int location = glGetUniformLocation(m_id, _name);
		if (location == -1)
		{
			std::cout << "Warning : uniform '" << _name << "' doesn't exist !" << std::endl;
		}

		m_uniformLocationCache[_name] = location;
		return location;
	}
}
