#include "MeshFIlterPooler.h"
#include "../Render/UniformManager.h"

MeshFilterPooler& MeshFilterPooler::Get()
{
	static MeshFilterPooler instance;
	return instance;
}

void MeshFilterPooler::Init()
{
	m_frameSends.resize(MAX_FRAMES_IN_FLIGHT, false);
}

MeshFilter* MeshFilterPooler::Add(const std::string& _name, const std::vector<Vertex>& _vertices,
                                  const std::vector<Index>& _indices)
{
	if (m_pooler.contains(_name))
	{
		return &m_pooler[_name];
	}

	MeshFilter newMeshFilter;
	newMeshFilter.vertexOffset = static_cast<uint32_t>(m_vertices.size());
	newMeshFilter.vertexCount = static_cast<uint32_t>(_vertices.size());
	newMeshFilter.indexOffset = static_cast<uint32_t>(m_indices.size());
	newMeshFilter.indexCount = static_cast<uint32_t>(_indices.size());

	m_pooler[_name] = newMeshFilter;

	m_vertices.insert(m_vertices.end(), _vertices.begin(), _vertices.end());
	m_indices.insert(m_indices.end(), _indices.begin(), _indices.end());

	m_meshFilterDirty = true;

	return &m_pooler[_name];
}

MeshFilter* MeshFilterPooler::GetMesh(const std::string& _name)
{
	if (m_pooler.contains(_name))
	{
		return &m_pooler[_name];
	}
	return nullptr;
}

void MeshFilterPooler::UpdateMeshFilter(IUniformManager* _uniformManager, uint32_t _frameIndex)
{
	if (m_meshFilterDirty)
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_frameSends[i] = false;
		}

		m_meshFilterDirty = false;
	}

	if (!m_frameSends[_frameIndex])
	{
		_uniformManager->StoreData("vertices", m_vertices.data(),
			ARRAY_SIZE_IN_BYTES(m_vertices), _frameIndex);

		_uniformManager->StoreData("indices", m_indices.data(),
			ARRAY_SIZE_IN_BYTES(m_indices), _frameIndex);

		m_frameSends[_frameIndex] = true;
	}
}

std::vector<Vertex> MeshFilterPooler::GetVerticies()
{
	return m_vertices;
}

std::vector<Index> MeshFilterPooler::GetIndices()
{
	return m_indices;
}
