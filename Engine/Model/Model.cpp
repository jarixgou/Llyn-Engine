#include "Model.h"

#include "../Utils/Utils.h"
#include "../Vertex/Vertex.h"
#include "../Texture/Texture.h"

namespace ENGINE_NAME
{
	Model::Model(const char* _filePath)
	{
		std::string content = Utils::GetFileContents(_filePath);

		m_json = json::parse(content);
		m_data = LoadBinaryFile();
	}

	Model::~Model()
	{

	}

	void Model::Draw(Shader& _shader, Camera& _camera)
	{

	}

	std::vector<unsigned char> Model::LoadBinaryFile()
	{
		// Create a place to store the raw text, and get the uri of the .bin file
		std::string bytesText;
		std::string uri = m_json["buffers"][0]["uri"];

		// Store raw text data into bytesText
		std::string fileStr = std::string(m_filePath);
		std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
		bytesText = Utils::GetFileContents((fileDirectory + uri).c_str());

		// Transform the raw text data into bytes and put them in a vector
		std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
		return data;
	}

	std::vector<float> Model::GetFloatsData(json& _accessor)
	{
		std::vector<float> floatVec;

		unsigned int bufferViewIndex = _accessor.value("bufferView", 1);
		unsigned int count = _accessor["count"];
		unsigned int accByteOffset = _accessor.value("byteOffset", 0);
		std::string type = _accessor["type"];

		json bufferView = m_json["bufferViews"][bufferViewIndex];
		unsigned int byteOffset = bufferView["byteOffset"];

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
		for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i)
		{
			unsigned char byteArray[4] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
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
		unsigned int byteOffset = bufferView["byteOffset"];

		unsigned int beginningOfData = byteOffset + accByteOffset;
		if (componentType == 5125)
		{
			for (unsigned int i = beginningOfData; i < beginningOfData + accByteOffset + count * 4; i)
			{
				unsigned char bytes[4] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
				unsigned int value = 0;
				std::memcpy(&value, bytes, sizeof(unsigned int));
				indices.push_back(static_cast<GLuint>(value));
			}
		}
		else if (componentType == 5123)
		{
			for (unsigned int i = beginningOfData; i < beginningOfData + accByteOffset + count * 2; i)
			{
				unsigned char bytes[2] = { m_data[i++], m_data[i++] };
				unsigned short value = 0;
				std::memcpy(&value, bytes, sizeof(unsigned short));
				indices.push_back(static_cast<GLuint>(value));
			}
		}
		else if (componentType == 5122)
		{
			for (unsigned int i = 0; i < beginningOfData + accByteOffset + count * 2; ++i)
			{
				unsigned char bytes[2] = { m_data[i++], m_data[i++] };
				short value = 0;
				std::memcpy(&value, bytes, sizeof(short));
				indices.push_back(static_cast<GLuint>(value));
			}
		}

		return indices;
	}

	std::vector<Texture> Model::GetTextures(json& _accessor)
	{
		std::vector<Texture> textures;

		std::string fileStr = std::string(m_filePath);
		std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

		for (unsigned int i = 0; i < m_json["images"].size(); ++i)
		{
			std::string texPath = m_json["images"][i]["uri"];

			bool skip = false;
			for (unsigned int j = 0; j < m_loadedTextures.size(); ++j)
			{
				if (m_loadedTextures[j].GetName() == texPath)
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
					Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", m_loadedTextures.size());
					textures.push_back(diffuse);
					m_loadedTextures.push_back(diffuse);
				}
				else if (texPath.find("metallicRoughness") != std::string::npos)
				{
					Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", m_loadedTextures.size());
					textures.push_back(specular);
					m_loadedTextures.push_back(specular);
				}
			}
		}
		return textures;
	}

	std::vector<Vertex> Model::AssembleVertices(std::vector<glm::vec3>& _positions, std::vector<glm::vec3>& _normals,
	                                            std::vector<glm::vec2>& _texUVs)
	{
		std::vector<Vertex> vertices;
		for (size_t i = 0; i < _positions.size(); i++)
		{
			Vertex vertex;
			vertex.position = _positions[i];
			vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
			vertex.normal = _normals[i];
			vertex.texUV = _texUVs[i];
			vertices.push_back(vertex);
		}
		return vertices;
	}

	std::vector<glm::vec2> Model::GroupFloatsVec2(const std::vector<float>& _data)
	{
		std::vector<glm::vec2> vectors;
		for (int i = 0; i < _data.size(); i)
		{
			vectors.emplace_back(_data[i++], _data[i++]);
		}
		return vectors;
	}

	std::vector<glm::vec3> Model::GroupFloatsVec3(const std::vector<float>& _data)
	{
		std::vector<glm::vec3> vectors;
		for (int i = 0; i < _data.size(); i)
		{
			vectors.emplace_back(_data[i++], _data[i++], _data[i++]);
		}
		return vectors;
	}

	std::vector<glm::vec4> Model::GroupFloatsVec4(const std::vector<float>& _data)
	{
		std::vector<glm::vec4> vectors;
		for (int i = 0; i < _data.size(); i)
		{
			vectors.emplace_back(_data[i++], _data[i++], _data[i++], _data[i++]);
		}
		return vectors;
	}
}