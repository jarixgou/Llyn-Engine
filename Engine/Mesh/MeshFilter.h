#ifndef MESH_FILTER__H
#define MESH_FILTER__H
#include <string>
#include <vector>

#include "../Vertex/Vertex.h"

namespace Llyn
{
	class VAO;
	class VBO;
	class EBO;

	struct MeshFilter
	{
		std::string name;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		VAO* vao;
		VBO* vbo;
		EBO* ebo;

		void Init();

		MeshFilter& operator=(const MeshFilter& _other);
	};
}

#endif