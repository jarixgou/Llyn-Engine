#ifndef TEXTURE__H
#define TEXTURE__H

#include <string>
#include <memory>
#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "../Asset/Asset.h"
#include "../Vector/Vector.h"


namespace Llyn
{
	class Shader;

	class Texture : public Asset
	{
	private:
		GLuint m_id;
		GLint m_slot;
		glm::vec2 m_size;

		std::string m_name;
	public:
		Texture() = default;
		Texture(const char* _filePath, GLuint _slot);
		~Texture() override;

		// Load a blank texture
		Texture(glm::vec2 _size);

		bool Load(const char* _path) override;
		bool Save() override;

		void Bind() const;
		void Unbind() const;

		void TexUnit(Shader& _shader, const char* _uniform);

		void SetRepeated(bool _repeated) const;
		void SetFilter(GLenum _minFilter, GLenum _magFilter) const;

		void SetSlot(GLuint _slot);

		void GenerateMipmap() const;
		const glm::vec2& GetSize() const;
		const std::string& GetName();
		const GLuint& GetId();
	};
}

#endif