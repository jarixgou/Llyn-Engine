#include "Model.h"

#include <utility>
#include <functional>
#include <iostream>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

#include "../Entities/ECS.h"
#include "../Entities/Entity.h"

#include "RessourceManager.h"
#include "Material.h"
#include "Texture.h"

#include "../Component/MeshRender.h"
#include "../Component/Transform.h"

#include "../Vertex.h"
#include "../Component/MeshFIlterPooler.h"
#include "../Vector/Vec3.h"

bool Model::Load(const std::string& _filePath)
{
	static constexpr fastgltf::Extensions supportedExtensions =
		fastgltf::Extensions::KHR_mesh_quantization |
		fastgltf::Extensions::KHR_texture_transform |
		fastgltf::Extensions::KHR_materials_variants;

	fastgltf::Parser parser(supportedExtensions);

	constexpr fastgltf::Options gltfOptions =
		fastgltf::Options::DontRequireValidAssetMember |
		fastgltf::Options::AllowDouble |
		fastgltf::Options::LoadExternalBuffers |
		fastgltf::Options::GenerateMeshIndices |
		fastgltf::Options::DecomposeNodeMatrices;

	auto gltfFile = fastgltf::MappedGltfFile::FromPath(_filePath);
	if (!static_cast<bool>(gltfFile))
	{
		// Print error message with logger 
		// Failed to open GLTF file !
		// fastgltf::getErrorMessage(gltfFile.error());
		return false;
	}

	size_t lastSlash = _filePath.find_last_of("/\\");
	if (lastSlash == std::string::npos)
	{
		lastSlash = 0;
	}

	size_t lastDot = _filePath.find_last_of('.');
	if (lastDot == std::string::npos)
	{
		// TODO: print error with logger
		return false;
	}

	m_rootDir = _filePath.substr(0, lastSlash + 1);
	m_name = _filePath.substr(lastSlash, lastDot);

	auto asset = parser.loadGltf(gltfFile.get(), m_rootDir, gltfOptions);
	if (asset.error() != fastgltf::Error::None)
	{
		// TODO: print error message with logger
		return false;
	}

	fastgltf::Asset gltfAsset = std::move(asset.get());

	m_checkedID.reserve(gltfAsset.nodes.size());

	const size_t sceneIndex = gltfAsset.defaultScene.value_or(0);
	for (size_t i = 0; i < gltfAsset.scenes[sceneIndex].nodeIndices.size(); i++)
	{
		Mesh rootModel;

		Transform transform;
		ParseNode(&rootModel, gltfAsset, i, transform);

		m_rootModels.emplace_back(rootModel);
	}
}

void Model::Unload()
{

}

bool Model::Add(const std::string& _filePath, IRessource* _ressource)
{
	return false;
}

void Model::SetModel(ECS* _ecs, Entity* _entity)
{
	LLYN_ASSERT(_ecs != nullptr || _entity != nullptr);

	if (_entity->id == TOMBSTONE_ENTITY)
	{
		return;
	}

	_entity->name = m_name;

	Transform transform;
	_ecs->AddComponents<Transform>(_entity->id, transform);

	_entity->childs.resize(m_rootModels.size());
	for (size_t i = 0; i < m_rootModels.size(); i++)
	{
		_entity->childs[i].parentID = _entity->id;
		ParseRootModel(_ecs, &_entity->childs[i], &m_rootModels[i]);
	}
}

void Model::ParseRootModel(ECS* _ecs, Entity* _entity, const Mesh* _rootModel)
{
	LLYN_ASSERT(_ecs != nullptr || _entity != nullptr);

	_entity->id = _ecs->CreateEntity();
	_entity->name = _rootModel->name;

	if (_rootModel->meshFilter != nullptr)
	{
		_ecs->AddComponents<MeshFilter>(_entity->id, *_rootModel->meshFilter);
	}
	if (_rootModel->meshRender != nullptr)
	{
		_ecs->AddComponents<MeshRender>(_entity->id, *_rootModel->meshRender);
	}
	if (_rootModel->transform != nullptr)
	{
		_ecs->AddComponents<Transform>(_entity->id, *_rootModel->transform);
	}

	_entity->childs.resize(_rootModel->subMeshes.size());
	for (size_t i = 0; i < _rootModel->subMeshes.size(); i++)
	{
		_entity->childs[i].parentID = _entity->id;
		ParseRootModel(_ecs, &_entity->childs[i], &_rootModel->subMeshes[i]);
	}
}

void Model::ParseNode(Mesh* _rootModel, const fastgltf::Asset& _gltfAsset, size_t _nodeIndex, Transform& _parentTransform)
{
	if (_rootModel == nullptr)
	{
		return;
	}

	bool checked = false;
	for (size_t i = 0; i < m_checkedID.size(); i++)
	{
		if (_nodeIndex == m_checkedID[i])
		{
			checked = true;
		}
	}

	const std::string nodeName = m_name + "_" + std::string(_gltfAsset.nodes[_nodeIndex].name);

	if (_gltfAsset.nodes[_nodeIndex].meshIndex.has_value() && !checked)
	{
		MeshFilter* meshFilter = MeshFilterPooler::Get().GetMesh(nodeName);
		RessourceID materialID = TOMBSTONE_RESOURCE;

		const size_t meshIndex = _gltfAsset.nodes[_nodeIndex].meshIndex.value();

		std::vector<Vertex> verticies;
		std::vector<Index> indices;

		for (size_t i = 0; i < _gltfAsset.meshes[meshIndex].primitives.size(); i++)
		{
			auto& primitives = _gltfAsset.meshes[meshIndex].primitives[i];

			size_t gltfUVIndex = 0;

			if (primitives.materialIndex.has_value())
			{
				const fastgltf::Material& gltfMaterial = _gltfAsset.materials[primitives.materialIndex.value()];

				if (gltfMaterial.pbrData.baseColorTexture.has_value())
				{
					if (gltfMaterial.pbrData.baseColorTexture.value().transform &&
						gltfMaterial.pbrData.baseColorTexture.value().transform->texCoordIndex.has_value())
					{
						gltfUVIndex = gltfMaterial.pbrData.baseColorTexture.value().transform->texCoordIndex.value();
					}
					else
					{
						gltfUVIndex = gltfMaterial.pbrData.baseColorTexture.value().texCoordIndex;
					}
				}

				materialID = GetMaterial(_gltfAsset, gltfMaterial);
			}
			else
			{
				materialID = RessourceManager::Get().GetRessourceID<Material>("Core/Materials/Lit.mat");
			}

			// Create a mesh filter 
			if (meshFilter == nullptr)
			{
				size_t startVertices = verticies.size();
				size_t startIndices = indices.size();

				// Create vertex data
				// Position
				const fastgltf::Attribute* positionAttribute = primitives.findAttribute("POSITION");
				if (positionAttribute != nullptr)
				{
					const fastgltf::Accessor& positionAccessor = _gltfAsset.accessors[positionAttribute->accessorIndex];

					if (positionAccessor.bufferViewIndex.has_value())
					{
						verticies.resize(startVertices + positionAccessor.count);

						fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(_gltfAsset, positionAccessor,
							[&](fastgltf::math::fvec3 _pos, size_t _idx)
							{
								verticies[startVertices + _idx].pos = { _pos.x(), _pos.y(), _pos.z() };
							});
					}
				}

				// Normal
				const fastgltf::Attribute* normalAttribute = primitives.findAttribute("NORMAL");
				if (normalAttribute != nullptr)
				{
					const fastgltf::Accessor& normalAccessor = _gltfAsset.accessors[normalAttribute->accessorIndex];

					if (normalAccessor.bufferViewIndex.has_value())
					{
						if (verticies.size() != normalAccessor.count)
						{
							verticies.resize(normalAccessor.count);
						}

						fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(_gltfAsset, normalAccessor,
							[&](fastgltf::math::fvec3 _normal, size_t _idx)
							{
								verticies[_idx].normal = { _normal.x(), _normal.y(), _normal.z() };
							});
					}
				}

				// UV coordinate
				std::string UVAttributeName = "TEXCOORD_" + std::to_string(gltfUVIndex);
				const fastgltf::Attribute* UVAttribute = primitives.findAttribute(UVAttributeName);
				if (UVAttribute != nullptr)
				{
					const fastgltf::Accessor& UVAccessor = _gltfAsset.accessors[UVAttribute->accessorIndex];

					if (UVAccessor.bufferViewIndex.has_value())
					{
						if (verticies.size() != UVAccessor.count)
						{
							verticies.resize(UVAccessor.count);
						}

						fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(_gltfAsset, UVAccessor,
							[&](fastgltf::math::fvec2 _uv, size_t _idx)
							{
								verticies[_idx].uv0 = { _uv.x(), _uv.y() };
							});
					}
				}

				// Create indicies
				assert(primitives.indicesAccessor.has_value());
				const fastgltf::Accessor& indexAccesor = _gltfAsset.accessors[primitives.indicesAccessor.value()];
				if (indexAccesor.componentType == fastgltf::ComponentType::UnsignedByte ||
					indexAccesor.componentType == fastgltf::ComponentType::UnsignedShort ||
					indexAccesor.componentType == fastgltf::ComponentType::UnsignedInt)
				{
					indices.resize(startIndices + indexAccesor.count);

					fastgltf::iterateAccessorWithIndex<Index>(_gltfAsset, indexAccesor,
						[&](Index _index, size_t _idx)
						{
							indices[startIndices + _idx] = _index;
						});
				}
			}
		}

		if (meshFilter == nullptr)
		{
			meshFilter = MeshFilterPooler::Get().Add(nodeName, verticies, indices);
		}

		MeshRender* meshRender = new MeshRender;
		meshRender->SetMaterial(materialID);

		_rootModel->meshFilter = meshFilter;
		_rootModel->meshRender = meshRender;
	}

	_rootModel->name = nodeName;

	// Get transform component
	Transform* transform = new Transform;
	std::visit(fastgltf::visitor{
		[&](const fastgltf::TRS& trs)
		{
			auto pos = trs.translation;
			auto rot = trs.rotation;
			auto scale = trs.scale;

			transform->pos = { pos.x(), pos.y(), pos.z() };
			transform->rot = { rot.w(), rot.x(), rot.y(), rot.z() };
			transform->scale = { scale.x(), scale.y(), scale.z() };
		},
		[&](const fastgltf::math::fmat4x4& _mat)
		{
			// Implement
		}}, _gltfAsset.nodes[_nodeIndex].transform);

	*transform *= _parentTransform;
	_rootModel->transform = transform;

	m_checkedID.emplace_back(_nodeIndex);

	if (_rootModel->subMeshes.size() != _gltfAsset.nodes[_nodeIndex].children.size())
	{
		_rootModel->subMeshes.resize(_gltfAsset.nodes[_nodeIndex].children.size());
		for (size_t i = 0; i < _gltfAsset.nodes[_nodeIndex].children.size(); i++)
		{
			ParseNode(&_rootModel->subMeshes[i], _gltfAsset, _gltfAsset.nodes[_nodeIndex].children[i], *transform);
		}
	}
}

RessourceID Model::GetMaterial(const fastgltf::Asset& _gltfAsset, const fastgltf::Material& _material)
{
	const std::string materialPath = m_rootDir + m_name + "_" + std::string(_material.name) + ".mat";

	RessourceID materialID = RessourceManager::Get().GetRessourceID<Material>(materialPath);
	if (materialID == TOMBSTONE_RESOURCE)
	{
		Material material;

		const RessourceID baseTextureID = GetTexture(_gltfAsset, _material.pbrData.baseColorTexture);
		if (baseTextureID != TOMBSTONE_RESOURCE)
		{
			material.SetBase(baseTextureID);
		}
		else
		{
			material.SetBase("Core/Textures/DefaultBase.png");
		}

		if (_material.specular != nullptr)
		{
			const RessourceID specularTextureID = GetTexture(_gltfAsset, _material.specular->specularTexture);
			if (specularTextureID != TOMBSTONE_RESOURCE)
			{
				material.SetSpecular(specularTextureID);
			}
			else
			{
				material.SetSpecular("Core/Textures/DefaultSpecular.png");
			}
		}
		else
		{
			material.SetSpecular("Core/Textures/DefaultSpecular.png");
		}

		const RessourceID normalTextureID = GetTexture(_gltfAsset, _material.normalTexture);
		material.SetNormal(normalTextureID);

		materialID = RessourceManager::Get().Add<Material>(materialPath, &material);
	}


	return materialID;
}

RessourceID Model::GetTexture(const fastgltf::Asset& _gltfAsset, const fastgltf::Optional<fastgltf::TextureInfo>& _textureInfo)
{
	RessourceID textureID = TOMBSTONE_RESOURCE;

	if (_textureInfo.has_value())
	{
		if (_gltfAsset.textures[_textureInfo.value().textureIndex].imageIndex.has_value())
		{
			const size_t imageIndex = _gltfAsset.textures[_textureInfo.value().textureIndex].imageIndex.value();

			std::visit(fastgltf::visitor{
					[](auto& _arg) {std::cout << "fallback: " << typeid(_arg).name() << std::endl;},
					[&](const fastgltf::sources::URI& _filePath) {
						assert(_filePath.fileByteOffset == 0);
						assert(_filePath.uri.isLocalPath());

						const std::string texturePath(_filePath.uri.path().begin(), _filePath.uri.path().end());

						textureID = RessourceManager::Get().GetRessourceID<Texture>(m_rootDir + texturePath);
					},
					[&](const fastgltf::sources::Vector& _vector)
					{
						std::cout << "Vector" << std::endl;
					},
					[&](const fastgltf::sources::BufferView& _view)
					{
						std::cout << "BufferView" << std::endl;
					}
				}, _gltfAsset.images[imageIndex].data);
		}
	}

	return textureID;
}

RessourceID Model::GetTexture(const fastgltf::Asset& _gltfAsset, const fastgltf::Optional<fastgltf::NormalTextureInfo>& _textureInfo)
{
	RessourceID textureID = TOMBSTONE_RESOURCE;

	if (_textureInfo.has_value())
	{
		if (_gltfAsset.textures[_textureInfo.value().textureIndex].imageIndex.has_value())
		{
			const size_t imageIndex = _gltfAsset.textures[_textureInfo.value().textureIndex].imageIndex.value();

			std::visit(fastgltf::visitor{
					[](auto& _arg) {},
					[&](fastgltf::sources::URI& _filePath) {
						assert(_filePath.fileByteOffset == 0);
						assert(_filePath.uri.isLocalPath());

						const std::string texturePath(_filePath.uri.path().begin(), _filePath.uri.path().end());

						textureID = RessourceManager::Get().GetRessourceID<Texture>(texturePath);
					}
				}, _gltfAsset.images[imageIndex].data);
		}
	}

	return textureID;
}