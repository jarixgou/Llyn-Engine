#ifndef TEXTURE__H
#define TEXTURE__H

#include <string>
#include <memory>
#include <glad/glad.h>

#include "../Asset/IAsset.h"
#include "../Vector/Vector.h"


namespace Llyn
{
	class Shader;

	class Texture : public IAsset
	{
	private:
		GLuint m_id;
		GLuint m_slot;
		Vector2u m_size;

		std::string m_name;
	public:
		Texture() = default;
		Texture(const char* _filePath, GLuint _slot);
		~Texture() override;

		bool Load(const char* _path) override;
		void* Get() override;

		void Bind() const;
		void Unbind() const;

		void TexUnit(Shader& _shader, const char* _uniform, GLuint _unit);

		void SetRepeated(bool _repeated) const;
		void SetFilter(GLenum _minFilter, GLenum _magFilter) const;

		void SetSlot(GLuint _slot);

		void GenerateMipmap() const;
		const Vector2u& GetSize() const;
		const std::string& GetName();
	};
}

#endif