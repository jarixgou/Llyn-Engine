#include "VK_Render.h"

#include <iostream>

#include "PipelineInfo.h"
#include "VK_Device.h"
#include "VK_FrameSync.h"
#include "VK_SwapChain.h"
#include "VK_Instance.h"
#include "VK_Pipeline.h"
#include "VK_DepthResources.h"
#include "VK_DescriptorManager.h"
#include "VK_Buffer.h"
#include "VK_ImGui.h"
#include "VK_RenderWindow.h"

#include "../MatricesData.h"
#include "../MetaData.h"

#include "../../Scene.h"
#include "../../Utils/Vulkan/VK_MemoryHelper.h"

#include "../../Entities/Entity.h"
#include "../../Entities/ECS.h"

#include "../../Component/Transform.h"
#include "../../Component/MeshFilter.h"
#include "../../Component/MeshRender.h"

#include "../../Asset/RessourceManager.h"
#include "../../Component/Camera.h"
#include "../../Component/MeshFIlterPooler.h"
#include "../../Component/Light/LightManager.h"
#include "../../Math/Math.h"

#include "../../Math/Matrix/Mat3.h"
#include "../../Vector/Vec3.h"

VK_Render::~VK_Render()
{

}

void VK_Render::Cleanup()
{
	m_renderWindow->Cleanup(m_device);
	delete m_renderWindow;
	m_renderWindow = nullptr;

	m_frameSync->Cleanup(m_device);
	delete m_frameSync;
	m_frameSync = nullptr;

	m_depthResources->Cleanup(m_device);
	delete m_depthResources;
	m_depthResources = nullptr;

	m_defaultPipeline->Cleanup(m_device);
	delete m_defaultPipeline;
	m_defaultPipeline = nullptr;

	m_device->Cleanup();
	delete m_device;
	m_device = nullptr;

	m_instance->Cleanup();
	delete m_instance;
	m_instance = nullptr;
}

void VK_Render::Init(IUniformManager** _uniformManger)
{
	AllocateVariable();

	m_instance->Init("Llyn");
	m_device->Init(m_instance);

	VK_Buffer::SetDevice(m_device);
	VK_MemoryHelper::SetDevice(m_device);
	Texture::SetDevice(m_device);

	m_renderWindow->Init(m_device, m_instance, true);

	m_frameSync->Init(m_device, m_renderWindow->GetSwapChain());

	VK_Buffer::SetFrameSync(m_frameSync);

	m_defaultPipeline->Init(m_device, m_renderWindow->GetSwapChain(), PipelineInfo{});

	VK_DescriptorManager* descriptorManager = new VK_DescriptorManager;
	descriptorManager->SetDevice(m_device);

	descriptorManager->Adds(m_defaultPipeline->GetDescriptorLayouts());

	*_uniformManger = descriptorManager;

	m_imgui->Init(m_instance, m_device, m_renderWindow->GetSwapChain(), m_frameSync);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		*m_indirectDrawBuff[i] = VK_Buffer::CreateIndirectBuffer(1000 * sizeof(VkDrawIndirectCommand));
	}
}

void VK_Render::Update(Scene* _scene, Camera* _camera, LightManager* _lightManager, IUniformManager* _uniformManager)
{
	MeshFilterPooler::Get().UpdateMeshFilter(_uniformManager, m_frameSync->GetFrameIndex());
	RessourceManager::Get().UpdateRessources(_uniformManager, m_frameSync->GetFrameIndex());
	_lightManager->UpdateLights(_scene->ecs, _uniformManager, m_frameSync->GetFrameIndex());

	uint32_t entityCount = _scene->ecs->GetEntityCount();

	m_metaDatas.clear();
	m_matricesDatas.clear();
	m_indirectCmds.clear();

	m_metaDatas.reserve(entityCount);
	m_matricesDatas.reserve(entityCount);

	m_indirectCmds.reserve(entityCount);

	uint32_t instance = 0;

	for (size_t i = 0; i < _scene->hierarchy.size(); i++)
	{
		ParseEntites(_scene->ecs, &_scene->hierarchy[i], Transform(), instance, _camera);
	}

	if (m_matricesDatas.empty())
	{
		m_matricesDatas.emplace_back();
	}

	if (m_metaDatas.empty())
	{
		m_metaDatas.emplace_back();
	}

	_uniformManager->StoreData("matrices", m_matricesDatas.data(),
		ARRAY_SIZE_IN_BYTES(m_matricesDatas), m_frameSync->GetFrameIndex());
	_uniformManager->StoreData("metaDatas", m_metaDatas.data(),
		ARRAY_SIZE_IN_BYTES(m_metaDatas), m_frameSync->GetFrameIndex());

	_uniformManager->PushUniform();
}

void VK_Render::Draw(Scene* _scene, Camera* _camera, Transform* _cameraTransform, LightManager* _lightManager, IUniformManager* _uniformManager)
{
	LLYN_ASSERT(_scene != nullptr && _uniformManager != nullptr);

	 auto [imageIndex, cmdBuff] = m_frameSync->Begin(m_device, m_renderWindow->GetSwapChain());

	Update(_scene, _camera, _lightManager, _uniformManager);

	if (!m_indirectCmds.empty())
	{
		memcpy(m_indirectDrawBuff[m_frameSync->GetFrameIndex()]->data, m_indirectCmds.data(), ARRAY_SIZE_IN_BYTES(m_indirectCmds));
	}

	m_frameSync->Reset(m_device);

	VK_Buffer::BeginCommandBuffer(cmdBuff, 0);

	m_renderWindow->Clear(cmdBuff, imageIndex);

	VkClearValue colorClear;
	colorClear.color = VkClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f };
	VkClearValue depthValue;
	depthValue.depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

	m_renderWindow->BeginRendering(cmdBuff, imageIndex, &colorClear, &depthValue);

	std::vector<VkDescriptorSet> descSets =
		static_cast<VK_DescriptorManager*>(_uniformManager)->GetDescriptorSets(m_frameSync->GetFrameIndex());

	vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_defaultPipeline->GetPipeline());
	vkCmdSetViewport(cmdBuff, 0, 1, m_renderWindow->GetWiewport());
	vkCmdSetScissor(cmdBuff, 0, 1, m_renderWindow->GetScissor());
	vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_defaultPipeline->GetPipelineLayout(), 0,
		static_cast<uint32_t>(descSets.size()), descSets.data(), 0, VK_NULL_HANDLE);

	VkPushConstantsInfo constantInfo{};
	constantInfo.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
	constantInfo.pNext = VK_NULL_HANDLE;
	constantInfo.layout = *m_defaultPipeline->GetPipelineLayout();
	constantInfo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	constantInfo.offset = 0;
	constantInfo.size = sizeof(Vec3f);
	constantInfo.pValues = &_cameraTransform->pos;
	vkCmdPushConstants2(cmdBuff, &constantInfo);

	if (!m_indirectCmds.empty())
	{
		vkCmdDrawIndirect(cmdBuff, m_indirectDrawBuff[m_frameSync->GetFrameIndex()]->buffer, 0,
			static_cast<uint32_t>(m_indirectCmds.size()), sizeof(VkDrawIndirectCommand));
	}

	m_renderWindow->EndRendering(cmdBuff);
	vkEndCommandBuffer(cmdBuff);

	VkCommandBuffer imGuiCmdBuff = m_imgui->Draw(m_renderWindow, m_frameSync->GetFrameIndex(), imageIndex);

	m_renderWindow->Display(imGuiCmdBuff, imageIndex);

	vkEndCommandBuffer(imGuiCmdBuff);

	std::vector<VkCommandBuffer> cmdBuffList = { cmdBuff, imGuiCmdBuff };;

	m_frameSync->End(m_device, m_renderWindow->GetSwapChain(), imageIndex, cmdBuffList);
}

void VK_Render::AllocateVariable()
{
	m_instance = new VK_Instance;
	m_device = new VK_Device;
	m_renderWindow = new VK_RenderWindow;
	m_frameSync = new VK_FrameSync;
	m_defaultPipeline = new VK_Pipeline;
	m_depthResources = new VK_DepthResources;

	m_imgui = new VK_ImGui;

	m_indirectDrawBuff.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		m_indirectDrawBuff[i] = new VK_Buffer;
	}
}

void VK_Render::ClearWindow(IUniformManager* _uniformManager, uint32_t _imageIndex)
{

}

void VK_Render::DisplayWindow(uint32_t _imageIndex)
{

}

void VK_Render::ParseEntites(ECS* _ecs, const Entity* _entity, Transform _transform,
	uint32_t& _instance, const Camera* _camera)
{
	LLYN_ASSERT(_ecs != nullptr || _entity != nullptr);

	if (_entity->id == TOMBSTONE_ENTITY)
	{
		return;
	}

	const Transform* baseTransform = _ecs->GetComponent<Transform>(_entity->id);
	_transform *= *baseTransform;

	if (_ecs->HasComponent<MeshFilter, MeshRender>(_entity->id))
	{
		const MeshFilter* meshFilter = _ecs->GetComponent<MeshFilter>(_entity->id);
		const MeshRender* meshRender = _ecs->GetComponent<MeshRender>(_entity->id);

		VkDrawIndirectCommand indirectCmd{};
		indirectCmd.firstInstance = _instance;
		indirectCmd.instanceCount = 1;
		indirectCmd.firstVertex = 0;
		indirectCmd.vertexCount = meshFilter->indexCount;

		m_indirectCmds.emplace_back(indirectCmd);

		MetaData metadata;
		metadata.indexCount = meshFilter->indexCount;
		metadata.indexOffset = meshFilter->indexOffset;
		metadata.vertexOffset = meshFilter->vertexOffset;
		metadata.materialID = meshRender->GetMaterialID();

		m_metaDatas.emplace_back(metadata);

		const Mat4 transfromMatrix = _transform.GetMatrix();
		const Mat4 baseMatrice4x4 = (_camera->GetProjMatrix() * _camera->GetViewMatrix()) * transfromMatrix;
		Mat3 baseMatrice3x3;
		baseMatrice3x3 = transfromMatrix;

		MatricesData matrices;
		matrices.mvp = baseMatrice4x4;
		matrices.normal = TransposeMat3(InverseMat3(baseMatrice3x3));

		m_matricesDatas.emplace_back(matrices);

		_instance += 1;
	}

	for (int i = 0; i < _entity->childs.size(); ++i)
	{
		ParseEntites(_ecs, &_entity->childs[i], _transform, _instance, _camera);
	}
}
