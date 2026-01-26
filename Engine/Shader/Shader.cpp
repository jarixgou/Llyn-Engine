#include "Shader.h"

#include "../Vector/Vector.h"

#include <fstream>
#include <ios>
#include <cerrno>
#include <iostream>

namespace ENGINE_NAME
{
	Shader::Shader(const char* _vertexShader, const char* _fragmentShader)
	{
		m_id = glCreateProgram();
		CompileShader(ReadFile(_vertexShader).c_str(), GL_VERTEX_SHADER);
		CompileShader(ReadFile(_fragmentShader).c_str(), GL_FRAGMENT_SHADER);
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

	void Shader::Activate() const
	{
		glUseProgram(m_id);
	}

	const GLuint& Shader::getID() const
	{
		return m_id;
	}

	void Shader::SetUniform(const GLuint& _value, const char* _name)
	{
		glUniform1ui(GetUniformLocation(_name), _value);
	}

	void Shader::SetUniform(const GLint& _value, const char* _name)
	{
		glUniform1i(GetUniformLocation(_name), _value);
	}

	void Shader::SetUniform(const GLfloat& _value, const char* _name)
	{
		glUniform1f(GetUniformLocation(_name), _value);
	}

	void Shader::SetUniform(const Vector2u& _value, const char* _name)
	{
		glUniform2ui(GetUniformLocation(_name), _value.x, _value.y);
	}

	void Shader::SetUniform(const Vector2i& _value, const char* _name)
	{
		glUniform2i(GetUniformLocation(_name), _value.x, _value.y);
	}

	void Shader::SetUniform(const Vector2f& _value, const char* _name)
	{
		glUniform2f(GetUniformLocation(_name), _value.x, _value.y);
	}

	void Shader::SetUniform(const Vector3u& _value, const char* _name)
	{
		glUniform3ui(GetUniformLocation(_name), _value.x, _value.y, _value.z);
	}

	void Shader::SetUniform(const Vector3i& _value, const char* _name)
	{
		glUniform3i(GetUniformLocation(_name), _value.x, _value.y, _value.z);
	}

	void Shader::SetUniform(const Vector3f& _value, const char* _name)
	{
		glUniform3f(GetUniformLocation(_name), _value.x, _value.y, _value.z);
	}

	void Shader::SetUniform(const Vector4u& _value, const char* _name)
	{
		glUniform4ui(GetUniformLocation(_name), _value.x, _value.y, _value.z, _value.w);
	}

	void Shader::SetUniform(const Vector4i& _value, const char* _name)
	{
		glUniform4i(GetUniformLocation(_name), _value.x, _value.y, _value.z, _value.w);
	}

	void Shader::SetUniform(const Vector4f& _value, const char* _name)
	{
		glUniform4f(GetUniformLocation(_name), _value.x, _value.y, _value.z, _value.w);
	}


	const std::string& Shader::ReadFile(const char* _filePath)
	{
		std::ifstream in(_filePath, std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
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
