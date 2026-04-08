#include "Model.h"

#include <iostream>
#include <assimp/postprocess.h>

#include "../Assert/Assert.h"
#include "../Asset/AssetManager.h"
#include "../Component/Mesh.h"
#include "../Asset/Material.h"
#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/Shader/Shader.h"
#include "../Vertex/Vertex.h"
#include "../Asset/Texture.h"
#include "../Camera/Camera.h"

namespace Llyn
{
	Model::Model(const char* _path)
	{
		LoadModel(_path);
	}

	Model::~Model()
	{
		for (auto& mesh : m_meshes)
		{
			DELETE_MEMORY(mesh);
		}
		m_models.clear();
	}

	void Model::Draw(Shader& _shader, Camera& _camera, glm::mat4 _model)
	{
		_shader.Activate();

		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			glm::mat4 finalMat = _model * m_models[i];
			/*m_meshes[i]->Draw(&_shader);*/
		}
	}

	void Model::LoadModel(std::string _path)
	{
		Assimp::Importer import;

		unsigned int flags =
			aiProcess_Triangulate;                

		const aiScene* scene = import.ReadFile(_path, flags);

		m_directory = _path.substr(0, _path.find_last_of('/'));

		std::cout << "Model directory: " << m_directory << std::endl;

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}

		glm::mat4 model(1.f);
		ProcessNode(scene->mRootNode, scene, model);
	}

	void Model::ProcessNode(aiNode* _node, const aiScene* _scene, glm::mat4& _parentModel)
	{
		aiMatrix4x4 aiTransform = _node->mTransformation;
		glm::mat4 nodeTransform;

		nodeTransform[0][0] = aiTransform.a1; nodeTransform[1][0] = aiTransform.a2;
		nodeTransform[2][0] = aiTransform.a3; nodeTransform[3][0] = aiTransform.a4;

		nodeTransform[0][1] = aiTransform.b1; nodeTransform[1][1] = aiTransform.b2;
		nodeTransform[2][1] = aiTransform.b3; nodeTransform[3][1] = aiTransform.b4;

		nodeTransform[0][2] = aiTransform.c1; nodeTransform[1][2] = aiTransform.c2;
		nodeTransform[2][2] = aiTransform.c3; nodeTransform[3][2] = aiTransform.c4;

		nodeTransform[0][3] = aiTransform.d1; nodeTransform[1][3] = aiTransform.d2;
		nodeTransform[2][3] = aiTransform.d3; nodeTransform[3][3] = aiTransform.d4;

		glm::mat4 globalTransform = _parentModel * nodeTransform * 0.3f;

		for (size_t i = 0; i < _node->mNumMeshes; ++i)
		{
			aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
			Mesh* processMesh = ProcessMesh(mesh, _scene);
			if (processMesh != nullptr)
			{
				m_meshes.push_back(ProcessMesh(mesh, _scene));
				m_models.push_back(globalTransform);
			}
		}
		for (size_t i = 0; i < _node->mNumChildren; ++i)
		{
			ProcessNode(_node->mChildren[i], _scene, globalTransform);
		}
	}

	Mesh* Model::ProcessMesh(aiMesh* _mesh, const aiScene* _scene) const
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		Material* material = nullptr;

		for (size_t i = 0; i < _mesh->mNumVertices; ++i)
		{
			Vertex vertex;

			vertex.position.x = _mesh->mVertices[i].x;
			vertex.position.y = _mesh->mVertices[i].y;
			vertex.position.z = _mesh->mVertices[i].z;

			vertex.normal.x = _mesh->mNormals[i].x;
			vertex.normal.y = _mesh->mNormals[i].y;
			vertex.normal.z = _mesh->mNormals[i].z;

			if (_mesh->mTextureCoords[0] != nullptr)
			{
				vertex.texUV.x = _mesh->mTextureCoords[0][i].x;
				vertex.texUV.y = _mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texUV = glm::vec2(0.f, 0.f);
			}

			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < _mesh->mNumFaces; ++i)
		{
			aiFace face = _mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		if (_mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = _scene->mMaterials[_mesh->mMaterialIndex];
			material = LoadMaterial(mat);
		}

		Mesh* mesh = nullptr;
		ALLOCATE_MEMORY(mesh, vertices, indices, material);
		return mesh;
	}

	Material* Model::LoadMaterial(aiMaterial* _mat) const
	{
		Material* material = nullptr;
		ALLOCATE_MEMORY(material);

		// Essayer DIFFUSE d'abord (format classique)
		if (_mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString texPath;
			_mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
			std::string fullPath = m_directory + '/' + texPath.C_Str();

			Texture* texture = AssetManager::Get()->GetAsset<Texture>(fullPath.c_str());
			if (texture != nullptr)
			{
				texture->SetSlot(0);
				material->baseMap = texture;
			}
		}
		// Essayer BASE_COLOR (format PBR/FBX moderne)
		else if (_mat->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
		{
			aiString texPath;
			_mat->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath);
			std::string fullPath = m_directory + '/' + texPath.C_Str();

			Texture* texture = AssetManager::Get()->GetAsset<Texture>(fullPath.c_str());
			if (texture != nullptr)
			{
				texture->SetSlot(0);
				material->baseMap = texture;
			}
		}

		// Charger la texture spéculaire si disponible
		if (_mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString texPath;
			_mat->GetTexture(aiTextureType_SPECULAR, 0, &texPath);
			std::string fullPath = m_directory + '/' + texPath.C_Str();

			Texture* texture = AssetManager::Get()->GetAsset<Texture>(fullPath.c_str());
			if (texture != nullptr)
			{
				texture->SetSlot(1);
				material->specularMap = texture;
			}
		}

		// Charger la normal map si disponible
		if (_mat->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiString texPath;
			_mat->GetTexture(aiTextureType_NORMALS, 0, &texPath);
			std::string fullPath = m_directory + '/' + texPath.C_Str();
			
			Texture* texture = AssetManager::Get()->GetAsset<Texture>(fullPath.c_str());
			if (texture != nullptr)
			{
				texture->SetSlot(2);
				material->normalMap = texture;
			}
		}
		else if (_mat->GetTextureCount(aiTextureType_HEIGHT) > 0)
		{
			aiString texPath;
			_mat->GetTexture(aiTextureType_HEIGHT, 0, &texPath);
			std::string fullPath = m_directory + '/' + texPath.C_Str();

			Texture* texture = AssetManager::Get()->GetAsset<Texture>(fullPath.c_str());
			if (texture != nullptr)
			{
				texture->SetSlot(2);
				material->normalMap = texture;
			}
		}

		if (material->baseMap == nullptr && material->normalMap == nullptr && material->specularMap)
		{
			DELETE_MEMORY(material);
		}

		return material;
	}
}
