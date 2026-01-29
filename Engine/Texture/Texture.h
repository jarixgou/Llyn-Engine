#ifndef TEXTURE__H
#define TEXTURE__H

#include <string>
#include <memory>
#include <glad/glad.h>

#include "../Vector/Vector.h"
#include "../Macro.h"

namespace ENGINE_NAME
{
	class Shader;

	class Texture
	{
	private:
		GLuint m_id;
		const char* m_type;
		GLuint m_slot;
		Vector2u m_size;

		std::string m_name;
	public:
		Texture(const char* _filePath, const char* _type, GLuint _slot);
		~Texture();

		void Bind() const;
		void Unbind() const;

		void TexUnit(Shader& _shader, const char* _uniform, GLuint& _unit);

		void SetRepeated(bool _repeated) const;
		void SetFilter(GLenum _minFilter, GLenum _magFilter) const;

		void SetType(const char* _type);
		const char* GetType() const;

		void GenerateMipmap() const;
		const Vector2u& GetSize() const;
		const std::string& GetName();
	};
}

#endif