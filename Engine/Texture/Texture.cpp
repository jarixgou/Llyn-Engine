#include "Texture.h"

#include <iostream>
#include <stb/stb_image.h>

namespace ENGINE_NAME
{
	Texture::Texture(const char* _filePath, GLenum _type, GLint _slot, GLenum _pixelType)
	{
		m_type = _type;
		m_slot = _slot;

		int widthImage;
		int heightImage;
		int numColChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* bytes = stbi_load(_filePath, &widthImage, &heightImage, &numColChannels, 0);
		if (!bytes)
		{
			std::cerr << "Failed to load texture: " << _filePath << std::endl;
			return;
		}

		m_size = { static_cast<GLuint>(widthImage), static_cast<GLuint>(heightImage) };

		GLenum format = 0;
		switch (numColChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default: ;
		}

		glGenTextures(1, &m_id);
		glActiveTexture(GL_TEXTURE0 + m_slot);
		Bind();

		SetFilter(GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER);
		SetRepeated(true);

		glTexImage2D(m_type, 0, GL_RGBA, widthImage, heightImage, 0, format, _pixelType, bytes);

		stbi_image_free(bytes);
		Unbind();
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	void Texture::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + m_slot);
		glBindTexture(m_type, m_id);
	}

	void Texture::Unbind() const
	{
		glBindTexture(m_type, 0);
	}

	void Texture::SetRepeated(bool _repeated) const
	{
		GLenum wrapMode = _repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE;
		glTexParameteri(m_type, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(m_type, GL_TEXTURE_WRAP_T, wrapMode);
	}

	void Texture::SetFilter(GLenum _minFilter, GLenum _magFilter) const
	{
		glTexParameteri(m_type, _minFilter, GL_NEAREST);
		glTexParameteri(m_type, _magFilter, GL_NEAREST);
	}

	void Texture::GenerateMipmap() const
	{
		glGenerateMipmap(m_type);
	}

	const Vector2u& Texture::GetSize() const
	{
		return m_size;
	}
}
