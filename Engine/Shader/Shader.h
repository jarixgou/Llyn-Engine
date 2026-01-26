#ifndef SHADER__H
#define SHADER__H

#include <string>
#include <unordered_map>
#include <glad/glad.h>

#include "../Macro.h"

namespace ENGINE_NAME
{
	struct Vector2u;
	struct Vector2i;
	struct Vector2f;

	struct Vector3u;
	struct Vector3i;
	struct Vector3f;

	struct Vector4u;
	struct Vector4i;
	struct Vector4f;

	class Shader
	{
	private:
		GLuint m_id;
		std::unordered_map<std::string, GLint> m_uniformLocationCache;
	public:
		// Load with file path
		Shader(const char* _vertexShader, const char* _fragmentShader);

		// Load with data
		Shader(const void* _vertexShader, const void* _fragmentShader);

		~Shader();

		void Activate() const;

		const GLuint& getID() const;

		void SetUniform(const GLuint& _value, const char* _name);
		void SetUniform(const GLint& _value, const char* _name);
		void SetUniform(const GLfloat& _value, const char* _name);

		void SetUniform(const Vector2u& _value, const char* _name);
		void SetUniform(const Vector2i& _value, const char* _name);
		void SetUniform(const Vector2f& _value, const char* _name);

		void SetUniform(const Vector3u& _value, const char* _name);
		void SetUniform(const Vector3i& _value, const char* _name);
		void SetUniform(const Vector3f& _value, const char* _name);

		void SetUniform(const Vector4u& _value, const char* _name);
		void SetUniform(const Vector4i& _value, const char* _name);
		void SetUniform(const Vector4f& _value, const char* _name);

	private:
		const std::string& ReadFile(const char* _filePath);
		void CompileShader(const char* _source, GLenum _type) const;
		int GetUniformLocation(const char* _name);
	};
}

#endif