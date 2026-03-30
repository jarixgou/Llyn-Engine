#ifndef MESH_FILTER_POOLER__H
#define MESH_FILTER_POOLER__H

#include <string>
#include <unordered_map>

namespace Llyn
{
	struct MeshFilter;

	class MeshFilterPooler
	{
	private:
		std::unordered_map<std::string, MeshFilter*> m_pooler;
		std::vector<std::string> m_nameMeshes;

		static MeshFilterPooler* s_instance;
	public:
		MeshFilterPooler() = default;
		~MeshFilterPooler();

		static MeshFilterPooler* Get();

		void AddMesh(MeshFilter* _mesh);
		MeshFilter* GetMesh(const char* _name);

		std::vector<std::string> GetAllNameMeshes();

		void Clear();
	};
}

#endif