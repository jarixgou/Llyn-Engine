#include "MeshHelper.h"

#include <vector>
#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>

#include "MeshFilter.h"
#include "MeshFilterPooler.h"
#include "../Memory/MemoryHelper.h"
#include "../Texture/Texture.h"
#include "../Vertex/Vertex.h"

#define RADIUS 0.1f
#define SECTOR_COUNT 16
#define STACK_COUNT 16

namespace Llyn
{
	MeshFilter* CreateCube()
	{

		std::vector<Vertex> vertices =
		{
			//					Coords									Normal							Color							UV
			{{-0.1f, -0.1f, 0.1f},		{0.f, 0.f, 1.f},	{1.f, 1.f, 1.f},		{0.f, 0.f}}, // 0
			{{0.1f, -0.1f, 0.1f},		{0.f, 0.f, 1.f},	{1.f, 1.f, 1.f},		{1.f, 0.f}}, // 1
			{{0.1f, 0.1f, 0.1f},		{0.f, 0.f, 1.f},	{1.f, 1.f, 1.f},		{1.f, 1.f}}, // 2
			{{-0.1f, 0.1f, 0.1f},		{0.f, 0.f, 1.f},	{1.f, 1.f, 1.f},		{0.f, 1.f}}, // 3

			{{0.1f, -0.1f, -0.1f},		{0.f, 0.f, -1.f},	{1.f, 1.f, 1.f},		{0.f, 0.f}}, // 4
			{{-0.1f, -0.1f, -0.1f},		{0.f, 0.f, -1.f},	{1.f, 1.f, 1.f},		{1.f, 0.f}}, // 5
			{{-0.1f, 0.1f, -0.1f},		{0.f, 0.f, -1.f},	{1.f, 1.f, 1.f},		{1.f, 1.f}}, // 6
			{{0.1f, 0.1f, -0.1f},		{0.f, 0.f, -1.f},	{1.f, 1.f, 1.f},		{0.f, 1.f}}, // 7

			{{0.1f, -0.1f, 0.1f},		{1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 0.f}}, // 8
			{{0.1f, -0.1f, -0.1f},		{1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 0.f}}, // 9
			{{0.1f, 0.1f, -0.1f},		{1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 1.f}}, // 10
			{{0.1f, 0.1f, 0.1f},		{1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 1.f}}, // 11

			{{-0.1f, -0.1f, -0.1f},		{-1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 0.f}}, // 12
			{{-0.1f, -0.1f, 0.1f},		{-1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 0.f}}, // 13
			{{-0.1f, 0.1f, 0.1f},		{-1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 1.f}}, // 14
			{{-0.1f, 0.1f, -0.1f},		{-1.f, 0.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 1.f}}, // 15

			{{-0.1f, 0.1f, 0.1f},		{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 0.f}}, // 16
			{{0.1f, 0.1f, 0.1f},		{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 0.f}}, // 17
			{{0.1f, 0.1f, -0.1f},		{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 1.f}}, // 18
			{{-0.1f, 0.1f, -0.1f},		{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 1.f}}, // 19

			{{-0.1f, -0.1f, -0.1f},		{0.f, -1.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 0.f}}, // 20
			{{0.1f, -0.1f, -0.1f},		{0.f, -1.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 0.f}}, // 21
			{{0.1f, -0.1f, 0.1f},		{0.f, -1.f, 0.f},	{1.f, 1.f, 1.f},		{1.f, 1.f}}, // 22
			{{-0.1f, -0.1f, 0.1f},		{0.f, -1.f, 0.f},	{1.f, 1.f, 1.f},		{0.f, 1.f}}, // 23
		};
		std::vector<uint32_t> indices =
		{
			0, 1, 2,	0, 2, 3,		// +Z

			4, 5, 6,	4, 6, 7,		// -Z

			8, 9, 10,	8, 10, 11,		// +X

			12, 13, 14,	12, 14, 15,		// -X

			16, 17, 18,	16, 18, 19,		// +Y

			20, 21, 22,	20, 22, 23		// -Y
		};

		MeshFilter* meshFilter = nullptr;
		ALLOCATE_MEMORY(meshFilter);

		meshFilter->name = "Cube";
		meshFilter->vertices = vertices;
		meshFilter->indices = indices;
		meshFilter->Init();
		return meshFilter;
	}

	MeshFilter* CreateSphere()
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture*> textures;

		Vertex vertex;
		vertex.color = glm::vec3(1.f, 1.f, 1.f);

		float xy = 0.f;

		constexpr float lengthNorm = 1.f / RADIUS;

		constexpr float sectorStep = 2 * glm::pi<float>() / SECTOR_COUNT;
		constexpr float stackStep = glm::pi<float>() / STACK_COUNT;

		float stackAngle = 0.f;
		float sectorAngle = 0.f;

		// Generate vertices
		for (int i = 0; i <= STACK_COUNT; ++i)
		{
			stackAngle = glm::pi<float>() / 2 - i * stackStep;
			xy = RADIUS * glm::cos(stackAngle);
			vertex.position.z = RADIUS * glm::sin(stackAngle);

			for (int j = 0; j <= SECTOR_COUNT; ++j)
			{
				sectorAngle = j * sectorStep;

				vertex.position.x = xy * glm::cos(sectorAngle);
				vertex.position.y = xy * glm::sin(sectorAngle);

				vertex.normal.x = vertex.position.x * lengthNorm;
				vertex.normal.y = vertex.position.y * lengthNorm;
				vertex.normal.z = vertex.position.z * lengthNorm;

				vertex.texUV.x = static_cast<float>(j) / SECTOR_COUNT;
				vertex.texUV.y = static_cast<float>(i) / STACK_COUNT;

				vertices.push_back(vertex);
			}
		}

		// Generate indices in CCW
		/*
		 * k1--k1+1
		 * |  / |
		 * | /  |
		 * k2--k2+1
		 */
		for (int i = 0; i < STACK_COUNT; ++i)
		{
			uint32_t k1 = i * (SECTOR_COUNT + 1);
			uint32_t k2 = k1 + SECTOR_COUNT + 1;

			for (int j = 0; j < SECTOR_COUNT; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != STACK_COUNT - 1)
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		MeshFilter* meshFilter = nullptr;
		ALLOCATE_MEMORY(meshFilter);

		meshFilter->name = "Sphere";
		meshFilter->vertices = vertices;
		meshFilter->indices = indices;
		meshFilter->Init();
		return meshFilter;
	}

	MeshFilter* CreatePlane()
	{
		std::vector<Vertex> vertices =
		{
			//		Coords						Normal				Color				UV
			{{-0.5f, 0.f, -0.5f},	{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},	{0.f, 0.f}}, // 0
			{{0.5f, 0.f, -0.5f},	{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},	{1.f, 0.f}}, // 1
			{{0.5f, 0.f, 0.5f},		{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},	{1.f, 1.f}}, // 2
			{{-0.5f, 0.f, 0.5f},	{0.f, 1.f, 0.f},	{1.f, 1.f, 1.f},	{0.f, 1.f}}, // 3
		};
		std::vector<uint32_t> indices
		{
			0, 2, 1,	0, 3, 2
		};

		MeshFilter* meshFilter = nullptr;
		ALLOCATE_MEMORY(meshFilter);

		meshFilter->name = "Plane";
		meshFilter->vertices = vertices;
		meshFilter->indices = indices;
		meshFilter->Init();
		return meshFilter;
	}
}