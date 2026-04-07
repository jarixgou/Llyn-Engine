#include "Texture.h"

#include <iostream>
#include <memory>
#include <stb/stb_image.h>

#include "../Render/OpenGL/Shader/Shader.h"
#include "../Utils/Utils.h"

namespace Llyn
{
	Texture::Texture(const char* _filePath, GLuint _slot)
	{
		Texture::Load(_filePath);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	Texture::Texture(glm::vec2 _size)
	{
		m_id = 0;
		m_slot = 0;
		m_size = _size;

		glGenTextures(1, &m_id);
		Bind();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		SetFilter(GL_LINEAR, GL_LINEAR);
		SetRepeated(false);
		Unbind();
	}

	bool Texture::Load(const char* _path)
	{
		m_slot = 0;
		m_id = 0;
		m_size = { 0, 0 };

		int widthImage;
		int heightImage;
		int numColChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* bytes = stbi_load(_path, &widthImage, &heightImage, &numColChannels, 0);
		if (!bytes)
		{
			return false;
		}

		m_size = { widthImage, heightImage };

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

		m_name = std::string(_path);
		m_name = m_name.substr(m_name.find_last_of("/\\") + 1);

		glGenTextures(1, &m_id);

		Bind();

		SetFilter(GL_NEAREST, GL_NEAREST);
		SetRepeated(true);

		glTexImage2D(GL_TEXTURE_2D, 0, format, widthImage, heightImage, 0, format, GL_UNSIGNED_BYTE, bytes);

		GenerateMipmap();

		stbi_image_free(bytes);
		Unbind();
		return true;
	}

	bool Texture::Save()
	{
		return true;
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

	void Texture::TexUnit(Shader& _shader, const char* _uniform)
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

	void Texture::SetSlot(GLuint _slot)
	{
		m_slot = _slot;
	}

	void Texture::GenerateMipmap() const
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	const glm::vec2& Texture::GetSize() const
	{
		return m_size;
	}

	const std::string& Texture::GetName()
	{
		return m_name;
	}

	const GLuint& Texture::GetId()
	{
		return m_id;
	}
}
