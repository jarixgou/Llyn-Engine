#include "Shader.h"

#include <fstream>
#include <ios>
#include <cerrno>
#include <iostream>

namespace Llyn
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
}
