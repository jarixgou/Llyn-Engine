#include "MeshFilter.h"

#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/EBO/EBO.h"
#include "../Render/OpenGL/VAO/VAO.h"
#include "../Render/OpenGL/VBO/VBO.h"

namespace Llyn
{
	void MeshFilter::Init()
	{	
		ALLOCATE_MEMORY(vao);
		ALLOCATE_MEMORY(vbo, vertices);
		ALLOCATE_MEMORY(ebo, indices);
		vao->Bind();

		vbo->Bind();
		ebo->Bind();

		vao->LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		vao->LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		vao->LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		vao->LinkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

		vao->Unbind();
		vbo->Unbind();
		ebo->Unbind();
	}

	MeshFilter& MeshFilter::operator=(const MeshFilter& _other)
	{
		if (this == &_other)
		{
			return *this;
		}

		vertices = _other.vertices;
		indices = _other.indices;

		vao = _other.vao;
		vbo = _other.vbo;
		ebo = _other.ebo;

		return *this;
	}
}
