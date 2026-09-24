#ifndef MESH_FILTER_POOLER__H
#define MESH_FILTER_POOLER__H
#include <string>
#include <unordered_map>

#include "MeshFilter.h"
#include "../Vertex.h"
#include "../LlynCore.h"

class MeshFilterPooler
{
private:
	std::unordered_map<std::string, MeshFilter> m_pooler;
	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;

	bool m_init = false;

	bool m_meshFilterDirty = false;
	std::vector<bool> m_frameSends;
public:
	static MeshFilterPooler& Get();

	void Init();

	MeshFilter* Add(const std::string& _name, const std::vector<Vertex>& _vertices, const std::vector<Index>& _indices);
	MeshFilter* GetMesh(const std::string& _name);

	void UpdateMeshFilter(IUniformManager* _uniformManager, uint32_t _frameIndex);
	
	std::vector<Vertex> GetVerticies();
	std::vector<Index> GetIndices();
};

#endif