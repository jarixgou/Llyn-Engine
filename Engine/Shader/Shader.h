#ifndef SHADER__H
#define SHADER__H

#include <string>
#include <glad/glad.h>

namespace Llyn
{
	class Shader
	{
	private:
		GLuint m_id;
	public:
		// Load with file path
		Shader(const char* _vertexShader, const char* _fragmentShader);

		// Load with data
		Shader(const void* _vertexShader, const void* _fragmentShader);

		~Shader();

		void Activate() const;

		const GLuint& getID() const;

	private:
		const std::string& ReadFile(const char* _filePath);
		void CompileShader(const char* _source, GLenum _type) const;
	};
}

#endif