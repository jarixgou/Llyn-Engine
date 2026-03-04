#include "VBO.h"

#include "../../../Vertex/Vertex.h"

namespace Llyn
{
	VBO::VBO(std::vector<Vertex>& _vertices)
	{
		glGenBuffers(1, &m_id);

		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);
	}

	VBO::~VBO()
	{
		glDeleteBuffers(1, &m_id);
	}

	void VBO::Bind()
	{
		glBindBuffer(1, m_id);
	}

	void VBO::Unbind()
	{
		glBindBuffer(0, m_id);
	}
}
