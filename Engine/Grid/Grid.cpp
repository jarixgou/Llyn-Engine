#include "Grid.h"

#include "../Render/OpenGL/Shader/Shader.h"
#include "../Asset/AssetManager.h"
#include "../Camera/Camera.h"
#include "../Mesh/MeshFilter.h"
#include "../Mesh/MeshFilterPooler.h"
#include "../Render/Render.h"

namespace Llyn
{
	Grid::Grid()
	{
		m_shader = AssetManager::Get()->GetAsset<Shader>("Core/Shaders/EndlessGrid.shader");
		m_meshFilter = MeshFilterPooler::Get()->GetMesh("Plane");
	}

	void Grid::Draw(Camera* _cam)
	{
		m_shader->Activate();

		Render::Get()->Draw(m_meshFilter->vao, m_meshFilter->indices);
	}
}