#ifndef RENDER__H
#define RENDER__H

#include <vector>
#include <glad/glad.h>
#include <glm/vec4.hpp>
#include "../Assert/Assert.h"

namespace Llyn
{
	class VAO;
	class EBO;
	class Camera;
	class Shader;

	void GLClearError();

	bool GLLogCall(const char* _function, const char* _file, int _line);

#define GL_CALL(x) GLClearError(); \
	x; \
	STATIC_ASSERT(GLLogCall(#x, __FILE__, __LINE__));

	enum class RenderMode
	{
		TRIANGLES,
		LINE,
		TRIANGLES_AND_LINE = TRIANGLES | LINE
	};

	class Render
	{
	private:
		RenderMode m_renderMode;

		static Render* s_instance;
	public:
		Render();
		~Render() = default;

		static Render* Get();

		void SetRenderMode(RenderMode _renderMode);
		RenderMode GetRenderMode();

		void Clear(glm::vec4 _color);
		void Draw(VAO* _vao, std::vector<GLuint>& _indices) const;
	};
}

#endif