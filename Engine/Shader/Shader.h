#ifndef SHADER__H
#define SHADER__H

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

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

		void SetUniform(const char* _name, const GLuint& _value);
		void SetUniform(const char* _name, const GLint& _value);
		void SetUniform(const char* _name, const GLfloat& _value);

		void SetUniform(const char* _name, const Vector2u& _value);
		void SetUniform(const char* _name, const Vector2i& _value);
		void SetUniform(const char* _name, const Vector2f& _value);

		void SetUniform(const char* _name, const Vector3u& _value);
		void SetUniform(const char* _name, const Vector3i& _value);
		void SetUniform(const char* _name, const glm::vec3& _value);

		void SetUniform(const char* _name, const Vector4u& _value);
		void SetUniform(const char* _name, const Vector4i& _value);
		void SetUniform(const char* _name, const glm::vec4& _value);

		void SetUniform(const char* _name, const GLfloat* _matrix4fv, int _size);
		void SetUniform(const char* _name, const glm::mat3& _value);
	private:
		void CompileShader(const char* _source, GLenum _type) const;
		int GetUniformLocation(const char* _name);
	};
}

#endif