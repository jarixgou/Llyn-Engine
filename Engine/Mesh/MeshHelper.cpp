#include "MeshHelper.h"

#include <vector>
#include <glad/glad.h>
#include <glm/geometric.hpp>

#include "Mesh.h"
#include "../Texture/Texture.h"
#include "../Vertex/Vertex.h"

#define RADIUS 0.1f
#define SECTOR_COUNT 16
#define STACK_COUNT 16

namespace Llyn
{
	Mesh CreateCube()
	{
		Vertex vertices[] =
		{//					Coords									Normal							Color							UV
			Vertex{glm::vec3(-0.1f, -0.1f, 0.1f),		glm::vec3(0.f, 0.f, 1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f)}, // 0
			Vertex{glm::vec3(0.1f, -0.1f, 0.1f),		glm::vec3(0.f, 0.f, 1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f)}, // 1
			Vertex{glm::vec3(0.1f, 0.1f, 0.1f),			glm::vec3(0.f, 0.f, 1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f)}, // 2
			Vertex{glm::vec3(-0.1f, 0.1f, 0.1f),		glm::vec3(0.f, 0.f, 1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f)}, // 3

			Vertex{glm::vec3(0.1f, -0.1f, -0.1f),		glm::vec3(0.f, 0.f, -1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f)}, // 4
			Vertex{glm::vec3(-0.1f, -0.1f, -0.1f),		glm::vec3(0.f, 0.f, -1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f)}, // 5
			Vertex{glm::vec3(-0.1f, 0.1f, -0.1f),		glm::vec3(0.f, 0.f, -1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f)}, // 6
			Vertex{glm::vec3(0.1f, 0.1f, -0.1f),		glm::vec3(0.f, 0.f, -1.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f)}, // 7

			Vertex{glm::vec3(0.1f, -0.1f, 0.1f),		glm::vec3(1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f)}, // 8
			Vertex{glm::vec3(0.1f, -0.1f, -0.1f),		glm::vec3(1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f)}, // 9
			Vertex{glm::vec3(0.1f, 0.1f, -0.1f),		glm::vec3(1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f)}, // 10
			Vertex{glm::vec3(0.1f, 0.1f, 0.1f),			glm::vec3(1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f)}, // 11

			Vertex{glm::vec3(-0.1f, -0.1f, -0.1f),		glm::vec3(-1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f)}, // 12
			Vertex{glm::vec3(-0.1f, -0.1f, 0.1f),		glm::vec3(-1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f)}, // 13
			Vertex{glm::vec3(-0.1f, 0.1f, 0.1f),		glm::vec3(-1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f)}, // 14
			Vertex{glm::vec3(-0.1f, 0.1f, -0.1f),		glm::vec3(-1.f, 0.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f)}, // 15

			Vertex{glm::vec3(-0.1f, 0.1f, 0.1f),		glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f)}, // 16
			Vertex{glm::vec3(0.1f, 0.1f, 0.1f),			glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f)}, // 17
			Vertex{glm::vec3(0.1f, 0.1f, -0.1f),		glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f)}, // 18
			Vertex{glm::vec3(-0.1f, 0.1f, -0.1f),		glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f)}, // 19

			Vertex{glm::vec3(-0.1f, -0.1f, -0.1f),		glm::vec3(0.f, -1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f)}, // 20
			Vertex{glm::vec3(0.1f, -0.1f, -0.1f),		glm::vec3(0.f, -1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f)}, // 21
			Vertex{glm::vec3(0.1f, -0.1f, 0.1f),		glm::vec3(0.f, -1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f)}, // 22
			Vertex{glm::vec3(-0.1f, -0.1f, 0.1f),		glm::vec3(0.f, -1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f)}, // 23
		};

		GLuint indices[] =
		{

			0, 2, 1,	0, 3, 2,

			4, 6, 5,	4, 7, 6,

			8, 10, 9,	8, 11, 10,

			12, 14, 13,	12, 15, 14,

			16, 18, 17,	16, 19, 18,

			20, 22, 21,	20, 23, 22
		};

		std::vector<Vertex> verticesList(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
		std::vector<GLuint> indiceList(indices, indices + sizeof(indices) / sizeof(GLuint));
		std::vector<Texture*> texturesList;

		Mesh mesh(verticesList, indiceList, nullptr);
		return mesh;
	}

	Mesh CreatSphere()
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
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

		// Generate indices in CW
		/*
		 * k1--k1+1
		 * |  / |
		 * | /  |
		 * k2--k2+1
		 */
		for (int i = 0; i < STACK_COUNT; ++i)
		{
			GLuint k1 = i * (SECTOR_COUNT + 1);
			GLuint k2 = k1 + SECTOR_COUNT + 1;

			for (int j = 0; j < SECTOR_COUNT; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k1 + 1);
					indices.push_back(k2);
				}

				if (i != STACK_COUNT - 1)
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2 + 1);
					indices.push_back(k2);
				}
			}
		}

		Mesh mesh(vertices, indices, nullptr);
		return mesh;
	}

	Mesh CreatePlane()
	{
		Vertex vertices[] =
		{//				Coords								Normal						Color						UV
			Vertex{glm::vec3(-0.1f, 0.f, 0.1f),		glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),	glm::vec2(0.f, 0.f)}, // 0
			Vertex{glm::vec3(0.1f, 0.f, 0.1f),		glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),	glm::vec2(1.f, 0.f)}, // 1
			Vertex{glm::vec3(0.1f, 0.f, -0.1f),		glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),	glm::vec2(1.f, 0.f)}, // 2
			Vertex{glm::vec3(-0.1f, 0.f, -0.1f),	glm::vec3(0.f, 1.f, 0.f),	glm::vec3(1.f, 1.f, 1.f),	glm::vec2(1.f, 0.f)}, // 3
		};

		GLuint indices[] =
		{
			3, 1, 0,	3, 2, 1
		};

		std::vector<Vertex> verticesList(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
		std::vector<GLuint> indicesList(indices, indices + sizeof(indices) / sizeof(GLuint));
		std::vector<Texture*> textureList;

		Mesh mesh(verticesList, indicesList, nullptr);
		return mesh;
	}
}
