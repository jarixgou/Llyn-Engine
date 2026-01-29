#include "Texture.h"

#include <iostream>
#include <memory>
#include <stb/stb_image.h>

#include "../Shader/Shader.h"

namespace ENGINE_NAME
{
	Texture::Texture(const char* _filePath, const char* _type, GLuint _slot)
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
		default:
			throw std::invalid_argument("Automatic Texture type recognition failed !");
		}

		m_name = std::string(_filePath);
		m_name = m_name.substr(m_name.find_last_of("/\\") + 1);

		glGenTextures(1, &m_id);

		glActiveTexture(GL_TEXTURE0 + m_slot);
		Bind();

		// Définit les paramètres avant l'image
		SetFilter(GL_NEAREST, GL_NEAREST);
		SetRepeated(true);

		glTexImage2D(GL_TEXTURE_2D, 0, format, widthImage, heightImage, 0, format, GL_UNSIGNED_BYTE, bytes);

		GenerateMipmap();


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
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::TexUnit(Shader& _shader, const char* _uniform, GLuint& _unit)
	{
		_shader.Activate();
		_shader.SetUniform(_uniform, m_slot);
	}

	void Texture::SetRepeated(bool _repeated) const
	{
		GLenum wrapMode = _repeated ? GL_REPEAT : GL_CLAMP_TO_EDGE;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	}

	void Texture::SetFilter(GLenum _minFilter, GLenum _magFilter) const
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);
	}

	void Texture::SetType(const char* _type)
	{
		m_type = _type;
	}

	const char* Texture::GetType() const
	{
		return m_type;
	}

	void Texture::GenerateMipmap() const
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	const Vector2u& Texture::GetSize() const
	{
		return m_size;
	}

	const std::string& Texture::GetName()
	{
		return m_name;
	}
}