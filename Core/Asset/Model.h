#ifndef MODEL__H
#define MODEL__H

#include <vector>
#include <fastgltf/types.hpp>

#include "../LlynCore.h"

#include "Ressource.h"

struct Mesh
{
	std::string name = DEFAULT_ENTITY_NAME;

	Transform* transform = nullptr;
	MeshRender* meshRender = nullptr;
	MeshFilter* meshFilter = nullptr;

	std::vector<Mesh> subMeshes;
};

class Model : public IRessource
{
private:
	std::string m_rootDir;
	std::string m_name;

	std::vector<size_t> m_checkedID;

	std::vector<Mesh> m_rootModels;
public:
	bool Load(const std::string& _filePath) override;
	void Unload() override;
	
	bool Add(const std::string& _filePath, IRessource* _ressource) override;

	void SetModel(ECS* _ecs, Entity* _entity);

private:
	void ParseRootModel(ECS* _ecs, Entity* _entity, const Mesh* _rootModel);
	void ParseNode(Mesh* _rootModel, const fastgltf::Asset& _gltfAsset, size_t _nodeIndex, Transform& _parentTransform);
	
	RessourceID GetMaterial(const fastgltf::Asset& _gltfAsset, const fastgltf::Material& _material);
	RessourceID GetTexture(const fastgltf::Asset& _asset, const fastgltf::Optional<fastgltf::TextureInfo>& _textureInfo);
	RessourceID GetTexture(const fastgltf::Asset& _asset, const fastgltf::Optional<fastgltf::NormalTextureInfo>& _textureInfo);
};

#endif