#ifndef TEXTURE__H
#define TEXTURE__H

#include <glad/glad.h>

#include "../Vector/Vector.h"
#include "../Macro.h"

namespace ENGINE_NAME
{
	class Texture
	{
	private:
		GLuint m_id;
		GLenum m_type;
		Vector2u m_size;
	public:
		Texture(const char* _filePath, GLenum _type, GLenum _slot, GLenum _pixelType);
		~Texture();

		void Bind() const;
		void Unbind() const;

		void SetRepeated(bool _repeated) const;
		void SetFilter(GLenum _minFilter, GLenum _magFilter) const;
		void GenerateMipmap() const;
		const Vector2u& GetSize() const;
	};
}

#endif