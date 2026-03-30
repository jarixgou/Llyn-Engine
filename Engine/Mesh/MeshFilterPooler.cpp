#include "MeshFilterPooler.h"

#include "MeshFilter.h"
#include "MeshHelper.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	MeshFilterPooler* MeshFilterPooler::s_instance = nullptr;

	MeshFilterPooler::~MeshFilterPooler()
	{
		for (auto & [name, obj] : m_pooler)
		{
			delete obj;
		}
		m_pooler.clear();
		m_nameMeshes.clear();
	}

	MeshFilterPooler* MeshFilterPooler::Get()
	{
		if (s_instance == nullptr)
		{
			ALLOCATE_MEMORY(s_instance);

			// Initialize the default mesh
			s_instance->AddMesh(CreateCube());
			s_instance->AddMesh(CreatePlane());
			s_instance->AddMesh(CreateSphere());
		}
		return s_instance;
	}

	void MeshFilterPooler::AddMesh(MeshFilter* _mesh)
	{
		if (m_pooler.contains(_mesh->name))
		{
			return;
		}
		m_nameMeshes.emplace_back(_mesh->name);
		m_pooler.insert({ _mesh->name, _mesh });
	}

	MeshFilter* MeshFilterPooler::GetMesh(const char* _name)
	{
		if (m_pooler.contains(_name))
		{
			return m_pooler[_name];
		}

		return nullptr;
	}

	std::vector<std::string> MeshFilterPooler::GetAllNameMeshes()
	{
		return m_nameMeshes;
	}

	void MeshFilterPooler::Clear()
	{
		for (auto& [key, mesh] : m_pooler)
		{
			DELETE_MEMORY(mesh);
		}
		m_pooler.clear();
	}
}
