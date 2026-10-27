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
#include "../Math/Math.h"

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

	m_rootDir = _filePath.substr(0, lastSlash + 1);
	m_name = _filePath.substr(lastSlash + 1);

	size_t lastDot = m_name.find_last_of('.');
	if (lastDot == std::string::npos)
	{
		// TODO: print error with logger
		return false;
	}
	m_name = m_name.substr(0, lastDot);

	auto asset = parser.loadGltf(gltfFile.get(), m_rootDir, gltfOptions);
	if (asset.error() != fastgltf::Error::None)
	{
		// TODO: print error message with logger
		return false;
	}

	fastgltf::Asset gltfAsset = std::move(asset.get());

	const size_t sceneIndex = gltfAsset.defaultScene.value_or(0);
	for (size_t i = 0; i < gltfAsset.scenes[sceneIndex].nodeIndices.size(); i++)
	{
		Mesh rootModel;

		Transform transform;
		ParseNode(&rootModel, gltfAsset, gltfAsset.scenes[sceneIndex].nodeIndices[i], transform);

		m_rootModels.emplace_back(rootModel);
	}

	return true;
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

	const std::string nodeName = m_name + "_" + std::string(_gltfAsset.nodes[_nodeIndex].name);

	size_t primitiveCount = 0;

	if (_gltfAsset.nodes[_nodeIndex].meshIndex.has_value())
	{
		const size_t meshIndex = _gltfAsset.nodes[_nodeIndex].meshIndex.value();

		primitiveCount = _gltfAsset.meshes[meshIndex].primitives.size();

		_rootModel->subMeshes.resize(primitiveCount + _gltfAsset.nodes[_nodeIndex].children.size());

		for (size_t i = 0; i < primitiveCount; i++)
		{
			const fastgltf::Primitive& primitives = _gltfAsset.meshes[meshIndex].primitives[i];

			Mesh& sub = _rootModel->subMeshes[i];
			sub.name = nodeName + "_" + std::to_string(i);
			sub.transform = new Transform;

			RessourceID materialID = RessourceManager::Get().GetRessourceID<Material>("Core/Materials/Lit.mat");;

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

				materialID = GetMaterial(_gltfAsset, gltfMaterial, primitives.materialIndex.value());
			}

			std::string meshFilterName = m_rootDir + m_name + "_mesh" + std::to_string(meshIndex) +
				"_prim" + std::to_string(i);

			MeshFilter* meshFilter = new MeshFilter;
			*meshFilter = MeshFilterPooler::Get().GetMesh(meshFilterName);

			// Create a mesh filter 
			if (meshFilter->name == "None")
			{
				std::vector<Vertex> verticies;
				std::vector<Index> indices;
				LoadPrimitive(_gltfAsset, primitives, gltfUVIndex, verticies, indices);
				*meshFilter = MeshFilterPooler::Get().Add(meshFilterName, verticies, indices);
			}

			MeshRender* meshRender = new MeshRender;
			meshRender->SetMaterial(materialID);

			sub.meshFilter = meshFilter;
			sub.meshRender = meshRender;
		}
	}
	else
	{
		_rootModel->subMeshes.resize(_gltfAsset.nodes[_nodeIndex].children.size());
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
		} }, _gltfAsset.nodes[_nodeIndex].transform);

	*transform = *transform * _parentTransform;
	_rootModel->transform = transform;

	for (size_t i = 0; i < _gltfAsset.nodes[_nodeIndex].children.size(); i++)
	{
		ParseNode(&_rootModel->subMeshes[primitiveCount + i], _gltfAsset, _gltfAsset.nodes[_nodeIndex].children[i], *transform);
	}
}

void Model::LoadPrimitive(const fastgltf::Asset& _asset, const fastgltf::Primitive& _primitive,
	const size_t& _uvIndex, std::vector<Vertex>& _outVertices, std::vector<Index>& _outIndices)
{
	const fastgltf::Attribute* positionAttrib = _primitive.findAttribute("POSITION");
	if (positionAttrib == nullptr)
	{
		return;
	}

	// Get position
	const fastgltf::Accessor& positionAccessor = _asset.accessors[positionAttrib->accessorIndex];
	if (positionAccessor.bufferViewIndex.has_value())
	{
		_outVertices.resize(positionAccessor.count);

		fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(_asset, positionAccessor,
			[&](fastgltf::math::fvec3 _pos, size_t _idx)
			{
				_outVertices[_idx].pos = { _pos.x(), _pos.y(), _pos.z() };
			});
	}

	// Get normal
	const fastgltf::Attribute* normalAttrib = _primitive.findAttribute("NORMAL");
	if (normalAttrib != nullptr)
	{
		const fastgltf::Accessor& accessor = _asset.accessors[normalAttrib->accessorIndex];

		if (accessor.bufferViewIndex.has_value())
		{
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(_asset, accessor,
				[&](fastgltf::math::fvec3 _normal, size_t _idx)
				{
					_outVertices[_idx].normal = { _normal.x(), _normal.y(), _normal.z() };
				});
		}
	}

	// Get UV
	std::string uvAttributeName = "TEXCOORD_" + std::to_string(_uvIndex);
	const fastgltf::Attribute* uvAttrib = _primitive.findAttribute(uvAttributeName);
	if (uvAttrib != nullptr)
	{
		const fastgltf::Accessor& accessor = _asset.accessors[uvAttrib->accessorIndex];

		if (accessor.bufferViewIndex.has_value())
		{
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(_asset, accessor,
				[&](fastgltf::math::fvec2 _uv, size_t _idx)
				{
					_outVertices[_idx].uv0 = { _uv.x(), _uv.y() };
				});
		}
	}

	// Get indices
	assert(_primitive.indicesAccessor.has_value());
	const fastgltf::Accessor& indexAccessor = _asset.accessors[_primitive.indicesAccessor.value()];
	if (indexAccessor.componentType == fastgltf::ComponentType::UnsignedByte ||
		indexAccessor.componentType == fastgltf::ComponentType::UnsignedShort ||
		indexAccessor.componentType == fastgltf::ComponentType::UnsignedInt)
	{
		_outIndices.resize(indexAccessor.count);

		fastgltf::iterateAccessorWithIndex<Index>(_asset, indexAccessor,
			[&](Index _index, size_t _idx)
			{
				_outIndices[_idx] = _index;
			});
	}

	// Get tangent
	const fastgltf::Attribute* tangentAttribute = _primitive.findAttribute("TANGENT");
	if (tangentAttribute != nullptr)
	{
		const fastgltf::Accessor& accessor = _asset.accessors[tangentAttribute->accessorIndex];

		if (accessor.type == fastgltf::AccessorType::Vec4)
		{
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(_asset, accessor,
				[&](fastgltf::math::fvec4 t, size_t i)
				{
					_outVertices[i].tangent = { t.x(), t.y(), t.z(), t.w() };
				});
		}
		else if (accessor.type == fastgltf::AccessorType::Vec3)
		{
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(_asset, accessor,
				[&](fastgltf::math::fvec3 t, size_t i)
				{
					_outVertices[i].tangent = { t.x(), t.y(), t.z(), 1.0f };
				});
		}
		else
		{
			ComputeTangent(_outVertices, _outIndices);
		}
	}
}

RessourceID Model::GetMaterial(const fastgltf::Asset& _gltfAsset, const fastgltf::Material& _material, const uint32_t _materialIndex)
{
	const std::string materialPath = m_rootDir + m_name + "_" +
		std::string(_material.name) + std::to_string(_materialIndex) + ".mat";

	RessourceID materialID = RessourceManager::Get().GetRessourceID<Material>(materialPath);

	if (materialID == TOMBSTONE_RESOURCE)
	{
		Material material{};

		const RessourceID baseTextureID = GetTexture(_gltfAsset, _material.pbrData.baseColorTexture);
		if (baseTextureID != TOMBSTONE_RESOURCE)
		{
			material.SetBase(baseTextureID);
		}
		else
		{
			material.SetBase("Core/Textures/DefaultBase.png");
		}

		if (_material.pbrData.metallicRoughnessTexture.has_value())
		{
			material.SetMode(MaterialMode::METALLIC);

			const RessourceID metallicTextureID = GetTexture(_gltfAsset, _material.pbrData.metallicRoughnessTexture);
			material.SetMetallic(metallicTextureID);

			material.SetSmoothness(_material.pbrData.roughnessFactor);
		}
		else if (_material.specular != nullptr)
		{
			material.SetMode(MaterialMode::SPECULAR);
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

		const RessourceID emissiveTextureID = GetTexture(_gltfAsset, _material.emissiveTexture);
		material.SetEmissive(emissiveTextureID);
		if (emissiveTextureID != TOMBSTONE_RESOURCE)
		{
			auto emissiveFactor = _material.emissiveFactor;
			material.SetEmissiveColor({ emissiveFactor.x(), emissiveFactor.y(), emissiveFactor.z() });
		}

		const RessourceID aoTextureID = GetTexture(_gltfAsset, _material.occlusionTexture);
		material.SetAO(aoTextureID);

		const RessourceID normalTextureID = GetTexture(_gltfAsset, _material.normalTexture);
		material.SetNormal(normalTextureID);

		materialID = RessourceManager::Get().Add<Material>(materialPath, &material);
	}


	return materialID;
}

RessourceID Model::GetTexture(const fastgltf::Asset& _asset, const fastgltf::Optional<fastgltf::TextureInfo>& _textureInfo)
{
	RessourceID textureID = TOMBSTONE_RESOURCE;

	if (_textureInfo.has_value())
	{
		if (_asset.textures[_textureInfo.value().textureIndex].imageIndex.has_value())
		{
			const size_t imageIndex = _asset.textures[_textureInfo.value().textureIndex].imageIndex.value();

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
				}, _asset.images[imageIndex].data);
		}
	}

	return textureID;
}

RessourceID Model::GetTexture(const fastgltf::Asset& _asset, const fastgltf::Optional<fastgltf::NormalTextureInfo>& _textureInfo)
{
	RessourceID textureID = TOMBSTONE_RESOURCE;

	if (_textureInfo.has_value())
	{
		if (_asset.textures[_textureInfo.value().textureIndex].imageIndex.has_value())
		{
			const size_t imageIndex = _asset.textures[_textureInfo.value().textureIndex].imageIndex.value();

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
				}, _asset.images[imageIndex].data);
		}
	}

	return textureID;
}

RessourceID Model::GetTexture(const fastgltf::Asset& _asset,
	const fastgltf::Optional<fastgltf::OcclusionTextureInfo>& _textureInfo)
{
	RessourceID textureID = TOMBSTONE_RESOURCE;

	if (_textureInfo.has_value())
	{
		if (_asset.textures[_textureInfo.value().textureIndex].imageIndex.has_value())
		{
			const size_t imageIndex = _asset.textures[_textureInfo.value().textureIndex].imageIndex.value();

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
				}, _asset.images[imageIndex].data);
		}
	}

	return textureID;
}

void Model::ComputeTangent(std::vector<Vertex>& _vertices, std::vector<Index> _indices)
{
	std::vector<Vec3f> tan(_vertices.size(), Vec3f{});
	std::vector<Vec3f> bit(_vertices.size(), Vec3f{});

	for (size_t i = 0; i + 2 < _indices.size(); i += 3)
	{
		const Index& i0 = _indices[i];
		const Index& i1 = _indices[i + 1];
		const Index& i2 = _indices[i + 2];

		const Vertex& v0 = _vertices[i0];
		const Vertex& v1 = _vertices[i1];
		const Vertex& v2 = _vertices[i2];

		const Vec3f dP0 = v1.pos - v0.pos;
		const Vec3f dP1 = v2.pos - v0.pos;

		const Vec2f dUv0 = v1.uv0 - v0.uv0;
		const Vec2f dUv1 = v2.uv0 - v0.uv0;

		const float f = 1.0f / (dUv0.x * dUv1.y - dUv1.x * dUv0.y);

		Vec3f t{};
		t.x = f * (dUv1.y * dP0.x - dUv0.y * dP1.x);
		t.y = f * (dUv1.y * dP0.y - dUv0.y * dP1.y);
		t.z = f * (dUv1.y * dP0.z - dUv0.y * dP1.z);
		
		Vec3f b{};
		b.x = f * (-dUv1.x * dP1.x + dUv0.x * dP1.x);
		b.y = f * (-dUv1.x * dP1.y + dUv0.x * dP1.y);
		b.z = f * (-dUv1.x * dP1.z + dUv0.x * dP1.z);

		tan[i0] += t;
		tan[i1] += t;
		tan[i2] += t;

		bit[i0] += b;
		bit[i1] += b;
		bit[i2] += b;
	}

	for (size_t i = 0; i < _vertices.size(); ++i)
	{
		const Vec3f& normal = _vertices[i].normal;
		Vec3f t = tan[i] - normal * Dot(normal, tan[i]);

		if (Dot(t, t) < 1e-12f)
		{
			Vec3f axis = abs(normal.x) < 0.9f ? Vec3f{ 1.0f, 0.0f, 0.0f } : Vec3f{ 0.0f, 1.0f, 0.0f };
			t = axis - normal * Dot(normal, axis);
		}
		t = Normalize(t);

		const float w = Dot(Cross(normal, t), bit[i]) < 0.0f ? -1.0f : 1.0f;
		_vertices[i].tangent = { t.x, t.y, t.z, w };
	}
}
