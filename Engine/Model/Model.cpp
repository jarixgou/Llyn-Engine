#include "Model.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "../Utils/Utils.h"
#include "../Vertex/Vertex.h"
#include "../Texture/Texture.h"
#include "../Mesh/Mesh.h"

namespace ENGINE_NAME
{
	Model::Model(const char* _filePath)
	{
		std::string content = Utils::GetFileContents(_filePath);

		m_filePath = _filePath;
		m_json = json::parse(content);
		m_data = LoadBinaryFile();

		TraverseNode(0);
	}

	Model::~Model()
	{
		for (auto& mesh : m_meshes)
		{
			delete mesh;
		}

		m_meshes.clear();
		m_loadedTextures.clear();
		m_translationsMeshes.clear();
		m_rotationsMeshes.clear();
		m_scalesMeshes.clear();
		m_matricesMeshes.clear();
	}

	void Model::Draw(Shader& _shader, Camera& _camera)
	{
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			m_meshes[i]->Draw(_shader, _camera, m_matricesMeshes[i]);
		}
	}

	void Model::LoadMesh(unsigned int _indMesh)
	{
		// Get all accessor indices
		unsigned int posAccInd = m_json["meshes"][_indMesh]["primitives"][0]["attributes"]["POSITION"];
		unsigned int normalAccInd = m_json["meshes"][_indMesh]["primitives"][0]["attributes"]["NORMAL"];
		unsigned int texAccInd = m_json["meshes"][_indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
		unsigned int indAccInd = m_json["meshes"][_indMesh]["primitives"][0]["indices"];

		// Use accessor indices to get all vertices components
		std::vector<float> posVec = GetFloats(m_json["accessors"][posAccInd]);
		std::vector<glm::vec3> positions = GroupFloatsVec3(posVec);
		std::vector<float> normalVec = GetFloats(m_json["accessors"][normalAccInd]);
		std::vector<glm::vec3> normals = GroupFloatsVec3(normalVec);
		std::vector<float> texVec = GetFloats(m_json["accessors"][texAccInd]);
		std::vector<glm::vec2> texUVs = GroupFloatsVec2(texVec);

		// Combine all the vertex components and also get the indices and textures
		std::vector<Vertex> vertices = AssembleVertices(positions, normals, texUVs);
		std::vector<GLuint> indices = GetIndices(m_json["accessors"][indAccInd]);
		std::vector<Texture*> textures = GetTextures();

		// Combine the vertices, indices, and textures into a mesh
		Mesh* newMesh = new Mesh(vertices, indices, textures);
		m_meshes.push_back(newMesh);
	}

	void Model::TraverseNode(unsigned int _nextNode, glm::mat4 _matrix)
	{
		json node = m_json["nodes"][_nextNode];

		glm::vec3 translations = glm::vec3(0.f, 0.f, 0.f);
		if (node.find("translation") != node.end())
		{
			float transValues[3] = { 0 };
			for (unsigned int i = 0; i < node["translation"].size(); ++i)
			{
				transValues[i] = node["translation"][i];
			}
			translations = glm::make_vec3(transValues);
		}

		glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
		if (node.find("rotation") != node.end())
		{
			float rotValues[4] =
			{
				node["rotation"][3],
				node["rotation"][0],
				node["rotation"][1],
				node["rotation"][2],
			};
			rotation = glm::make_quat(rotValues);
		}

		glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
		if (node.find("scale") != node.end())
		{
			float scaleValues[3];
			for (unsigned int i = 0; i < node["scale"].size(); ++i)
			{
				scaleValues[i] = node["scale"][i];
			}
			scale = glm::make_vec3(scaleValues);
		}

		glm::mat4 matNode = glm::mat4(1.f);
		if (node.find("matrix") != node.end())
		{
			float matValues[16] = { 0 };
			// ✅ Correction : utiliser node["matrix"].size() au lieu de node.size()
			for (unsigned int i = 0; i < node["matrix"].size(); ++i)
			{
				matValues[i] = node["matrix"][i];
			}
			matNode = glm::make_mat4(matValues);
		}

		glm::mat4 trans = glm::mat4(1.f);
		glm::mat4 rot = glm::mat4(1.f);
		glm::mat4 sca = glm::mat4(1.f);

		trans = glm::translate(trans, translations);
		rot = glm::mat4_cast(rotation);
		sca = glm::scale(sca, scale);

		glm::mat4 matNextNode = _matrix * trans * rot * sca;

		if (node.find("mesh") != node.end())
		{
			m_translationsMeshes.push_back(translations);
			m_rotationsMeshes.push_back(rotation);
			m_scalesMeshes.push_back(scale);
			m_matricesMeshes.push_back(matNextNode);

			LoadMesh(node["mesh"]);
		}

		if (node.find("children") != node.end())
		{
			for (unsigned int i = 0; i < node["children"].size(); ++i)
			{
				TraverseNode(node["children"][i], matNextNode);
			}
		}
	}

	std::vector<unsigned char> Model::LoadBinaryFile()
	{
		std::string bytesText;
		std::string uri = m_json["buffers"][0]["uri"];

		std::string fileStr = std::string(m_filePath);
		std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
		bytesText = Utils::GetFileContents((fileDirectory + uri).c_str());

		std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
		return data;
	}

	std::vector<float> Model::GetFloats(json& _accessor)
	{
		std::vector<float> floatVec;

		unsigned int bufferViewIndex = _accessor.value("bufferView", 1);
		unsigned int count = _accessor["count"];
		unsigned int accByteOffset = _accessor.value("byteOffset", 0);
		std::string type = _accessor["type"];

		json bufferView = m_json["bufferViews"][bufferViewIndex];
		unsigned int byteOffset = bufferView.value("byteOffset", 0);

		unsigned int numPerVert = 0;
		if (type == "SCALAR")
		{
			numPerVert = 1;
		}
		else if (type == "VEC2")
		{
			numPerVert = 2;
		}
		else if (type == "VEC3")
		{
			numPerVert = 3;
		}
		else if (type == "VEC4")
		{
			numPerVert = 4;
		}
		else
		{
			throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");
		}

		unsigned int beginningOfData = byteOffset + accByteOffset;
		unsigned int lengthOfData = count * 4 * numPerVert;
		for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
		{
			unsigned char byteArray[4] = { m_data[i], m_data[i + 1], m_data[i + 2], m_data[i + 3] };
			float value = 0;
			std::memcpy(&value, byteArray, sizeof(float));
			floatVec.push_back(value);
		}

		return floatVec;
	}

	std::vector<GLuint> Model::GetIndices(json& _accessor)
	{
		std::vector<GLuint> indices;

		unsigned int bufferViewIndex = _accessor.value("bufferView", 0);
		unsigned int count = _accessor["count"];
		unsigned int accByteOffset = _accessor.value("byteOffset", 0);
		unsigned int componentType = _accessor["componentType"];

		json bufferView = m_json["bufferViews"][bufferViewIndex];
		unsigned int byteOffset = bufferView.value("byteOffset", 0);

		unsigned int beginningOfData = byteOffset + accByteOffset;

		if (componentType == 5125) // UNSIGNED_INT
		{
			for (unsigned int i = beginningOfData; i < beginningOfData + count * 4; i += 4)
			{
				unsigned char bytes[4] = { m_data[i], m_data[i + 1], m_data[i + 2], m_data[i + 3] };
				unsigned int value = 0;
				std::memcpy(&value, bytes, sizeof(unsigned int));
				indices.push_back(static_cast<GLuint>(value));
			}
		}
		else if (componentType == 5123) // UNSIGNED_SHORT
		{
			for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i += 2)
			{
				unsigned char bytes[2] = { m_data[i], m_data[i + 1] };
				unsigned short value = 0;
				std::memcpy(&value, bytes, sizeof(unsigned short));
				indices.push_back(static_cast<GLuint>(value));
			}
		}
		else if (componentType == 5122) // SHORT
		{
			for (unsigned int i = beginningOfData; i < beginningOfData + count * 2; i += 2)
			{
				unsigned char bytes[2] = { m_data[i], m_data[i + 1] };
				short value = 0;
				std::memcpy(&value, bytes, sizeof(short));
				indices.push_back(static_cast<GLuint>(value));
			}
		}

		return indices;
	}

	std::vector<Texture*> Model::GetTextures()
	{
		std::vector<Texture*> textures;

		std::string fileStr = std::string(m_filePath);
		std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

		for (unsigned int i = 0; i < m_json["images"].size(); ++i)
		{
			std::string texPath = m_json["images"][i]["uri"];

			bool skip = false;
			for (unsigned int j = 0; j < m_loadedTextures.size(); ++j)
			{
				if (m_loadedTextures[j]->GetName() == texPath)
				{
					textures.push_back(m_loadedTextures[j]);
					skip = true;
					j = m_loadedTextures.size();
				}
			}

			if (!skip)
			{
				if (texPath.find("baseColor") != std::string::npos)
				{
					Texture* newTexture = new Texture((fileDirectory + texPath).c_str(), "diffuse", m_loadedTextures.size());
					m_loadedTextures.push_back(newTexture);
					textures.push_back(m_loadedTextures.back());
				}
				else if (texPath.find("metallicRoughness") != std::string::npos)
				{
					Texture* newTexture = new Texture((fileDirectory + texPath).c_str(), "specular", m_loadedTextures.size());
					m_loadedTextures.push_back(newTexture);
					textures.push_back(m_loadedTextures.back());
				}
			}
		}
		return textures;
	}

	std::vector<Vertex> Model::AssembleVertices
	(
		std::vector<glm::vec3>& _positions,
		std::vector<glm::vec3>& _normals,
		std::vector<glm::vec2>& _texUVs
	)
	{
		std::vector<Vertex> vertices;
		for (int i = 0; i < _positions.size(); i++)
		{
			vertices.push_back
			(
				Vertex
				{
					_positions[i],
					_normals[i],
					glm::vec3(1.0f, 1.0f, 1.0f),
					_texUVs[i]
				}
			);
		}
		return vertices;
	}

	std::vector<glm::vec2> Model::GroupFloatsVec2(const std::vector<float>& _data)
	{
		const unsigned int floatsPerVector = 2;

		std::vector<glm::vec2> vectors;
		for (unsigned int i = 0; i < _data.size(); i += floatsPerVector)
		{
			vectors.push_back(glm::vec2(0, 0));

			for (unsigned int j = 0; j < floatsPerVector; j++)
			{
				vectors.back()[j] = _data[i + j];
			}
		}
		return vectors;
	}
	std::vector<glm::vec3> Model::GroupFloatsVec3(const std::vector<float>& _data)
	{
		const unsigned int floatsPerVector = 3;

		std::vector<glm::vec3> vectors;
		for (unsigned int i = 0; i < _data.size(); i += floatsPerVector)
		{
			vectors.push_back(glm::vec3(0, 0, 0));

			for (unsigned int j = 0; j < floatsPerVector; j++)
			{
				vectors.back()[j] = _data[i + j];
			}
		}
		return vectors;
	}
	std::vector<glm::vec4> Model::GroupFloatsVec4(const std::vector<float>& _data)
	{
		const unsigned int floatsPerVector = 4;

		std::vector<glm::vec4> vectors;
		for (unsigned int i = 0; i < _data.size(); i += floatsPerVector)
		{
			vectors.push_back(glm::vec4(0, 0, 0, 0));

			for (unsigned int j = 0; j < floatsPerVector; j++)
			{
				vectors.back()[j] = _data[i + j];
			}
		}
		return vectors;
	}
}