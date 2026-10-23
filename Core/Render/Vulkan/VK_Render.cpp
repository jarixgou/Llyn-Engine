#include "VK_Render.h"

#include <iostream>

#include "PipelineInfo.h"
#include "VK_Device.h"
#include "VK_FrameSync.h"
#include "VK_Instance.h"
#include "VK_Pipeline.h"
#include "VK_DepthResources.h"
#include "VK_DescriptorManager.h"
#include "VK_Buffer.h"
#include "VK_ImGui.h"
#include "VK_RenderWindow.h"
#include "VK_SwapChain.h"
#include "VK_RenderGraph.h"

#include "../MatricesData.h"
#include "../MetaData.h"

#include "../../Scene.h"

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

#include "../../Vector/Vec3.h"

VK_Render::~VK_Render()
{

}

void VK_Render::Cleanup()
{
	m_renderWindow->Cleanup();
	delete m_renderWindow;
	m_renderWindow = nullptr;

	m_frameSync->Cleanup();
	delete m_frameSync;
	m_frameSync = nullptr;

	m_depthResources->Cleanup();
	delete m_depthResources;
	m_depthResources = nullptr;

	m_gBufferPipeline->Cleanup();
	delete m_gBufferPipeline;
	m_gBufferPipeline = nullptr;

	m_lightingPipeline->Cleanup();
	delete m_lightingPipeline;
	m_lightingPipeline = nullptr;

	m_frameBufferPipeline->Cleanup();
	delete m_frameBufferPipeline;
	m_frameBufferPipeline = nullptr;

	VK_Device::Get().Cleanup();

	m_instance->Cleanup();
	delete m_instance;
	m_instance = nullptr;
}

void VK_Render::Init(IUniformManager** _uniformManger)
{
	AllocateVariable();

	m_instance->Init("Llyn");
	VK_Device::Get().Init(m_instance);
	VK_VmaAllocatorWrapper::Get().Init(m_instance);

	m_renderWindow->Init(m_instance, true);

	m_frameSync->Init(m_renderWindow->GetSwapChain());

	VK_Buffer::SetFrameSync(m_frameSync);

	PipelineInfo gBufferPipelineInfo;
	gBufferPipelineInfo.shaderPath = "Core/shaders/IndirectG-Buffer.slang.spv";
	gBufferPipelineInfo.imageFormats = 
		{
			IMAGE_FORMAT_R8G8B8A8, IMAGE_FORMAT_R8G8B8A8, IMAGE_FORMAT_R8G8B8A8_SRGB,
			IMAGE_FORMAT_R8G8B8A8_SRGB
		};
	m_gBufferPipeline->Init(m_renderWindow->GetSwapChain(), gBufferPipelineInfo);

	PipelineInfo lightingPipelineInfo;
	lightingPipelineInfo.shaderPath = "Core/shaders/LightingPass.slang.spv";
	lightingPipelineInfo.imageFormats = { IMAGE_FORMAT_R8G8B8A8_SRGB };
	lightingPipelineInfo.renderFace = RenderFace::NONE;
	m_lightingPipeline->Init(m_renderWindow->GetSwapChain(), lightingPipelineInfo);

	LOGGER_INFO("FrameBuffer pipeline creation ! ");
	PipelineInfo frameBufferInfo;
	frameBufferInfo.shaderPath = "Core/Shaders/FrameBuffer.slang.spv";
	frameBufferInfo.imageFormats = { IMAGE_FORMAT_R8G8B8A8_SRGB };
	frameBufferInfo.renderFace = RenderFace::NONE;
	m_frameBufferPipeline->Init(m_renderWindow->GetSwapChain(), frameBufferInfo);

	VK_DescriptorManager* descriptorManager = new VK_DescriptorManager;
	
	descriptorManager->Adds("GBuffer", m_gBufferPipeline->GetDescriptorLayouts());
	descriptorManager->Adds("Lighting", m_lightingPipeline->GetDescriptorLayouts());
	descriptorManager->Adds("FrameBuffer", m_frameBufferPipeline->GetDescriptorLayouts());

	*_uniformManger = descriptorManager;

	m_imgui->Init(m_instance, m_renderWindow->GetSwapChain(), m_frameSync);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		*m_indirectDrawBuff[i] = VK_Buffer::CreateIndirectBuffer(1000 * sizeof(VkDrawIndirectCommand));
	}

	m_renderGraph->AddResource("gPosition", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		{ 1920, 1080 });
	m_renderGraph->AddResource("gNormal", VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		{ 1920, 1080 });
	m_renderGraph->AddResource("gAlbedo", VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		{ 1920, 1080 });
	m_renderGraph->AddResource("gSpecular", VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		{ 1920, 1080 });

	m_renderGraph->AddResource("Depth", VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
		{ 1920, 1080 });

	m_renderGraph->AddPass("GBuffer", {}, { "gPosition", "gNormal", "gAlbedo", "gSpecular", "Depth" },
		nullptr,
		[&](VkCommandBuffer _cmdBuff, IUniformManager* _manager, const uint32_t& _frameIndex)
		{
			std::vector<VK_Resource*> resources =
			{
				m_renderGraph->GetResource("gPosition"),
				m_renderGraph->GetResource("gNormal"),
				m_renderGraph->GetResource("gAlbedo"),
				m_renderGraph->GetResource("gSpecular"),
				m_renderGraph->GetResource("Depth")
			};

			std::vector<VkRenderingAttachmentInfo> colorAttachments(4);
			for (size_t i = 0; i < colorAttachments.size(); ++i)
			{
				VkRenderingAttachmentInfo* color = &colorAttachments[i];
				color->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
				color->pNext = VK_NULL_HANDLE;
				color->imageView = resources[i]->image.m_imageView;
				color->imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				color->resolveMode = VK_RESOLVE_MODE_NONE;
				color->resolveImageView = VK_NULL_HANDLE;
				color->resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				color->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				color->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			}

			VkRenderingAttachmentInfo depthAttachment{};
			depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.pNext = VK_NULL_HANDLE;
			depthAttachment.imageView = resources[resources.size() - 1]->image.m_imageView;
			depthAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
			depthAttachment.resolveImageView = VK_NULL_HANDLE;
			depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			VkRenderingInfo renderingInfo{};
			renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			renderingInfo.pNext = VK_NULL_HANDLE;
			renderingInfo.renderArea = { { 0, 0 }, *m_renderWindow->GetSwapChain()->GetExtent() };
			renderingInfo.layerCount = 1;
			renderingInfo.viewMask = 0;
			renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
			renderingInfo.pColorAttachments = colorAttachments.data();
			renderingInfo.pDepthAttachment = &depthAttachment;

			std::vector<VkDescriptorSet> descSets = static_cast<VK_DescriptorManager*>(_manager)->GetDescriptorSets("GBuffer", _frameIndex);

			vkCmdBeginRendering(_cmdBuff, &renderingInfo);
			vkCmdBindPipeline(_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_gBufferPipeline->GetPipeline());
			vkCmdSetViewport(_cmdBuff, 0, 1, m_renderWindow->GetWiewport());
			vkCmdSetScissor(_cmdBuff, 0, 1, m_renderWindow->GetScissor());
			vkCmdBindDescriptorSets(_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_gBufferPipeline->GetPipelineLayout(),
				0, static_cast<uint32_t>(descSets.size()), descSets.data(),
				0, VK_NULL_HANDLE);

			vkCmdDrawIndirect(_cmdBuff, m_indirectDrawBuff[_frameIndex]->buffer, 0,
				static_cast<uint32_t>(m_indirectCmds.size()), sizeof(VkDrawIndirectCommand));

			vkCmdEndRendering(_cmdBuff);
		}
	);

	m_renderGraph->AddResource("LightingPass", VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		{ 1920, 1080 });

	m_renderGraph->AddPass("Lighting", { "GPosition", "GNormal", "GAlbedo", "GSpecular" }, { "LightingPass" },
		[&](IUniformManager* _manager, const uint32_t _frameIndex)
		{
			std::vector<VK_Resource*> resources =
			{
				m_renderGraph->GetResource("gPosition"),
				m_renderGraph->GetResource("gNormal"),
				m_renderGraph->GetResource("gAlbedo"),
				m_renderGraph->GetResource("gSpecular")
			};

			VK_DescriptorManager* manager = static_cast<VK_DescriptorManager*>(_manager);

			for (int i = 0; i < resources.size(); ++i)
			{
				manager->StoreImage("Lighting", resources[i]->name, _frameIndex, resources[i]->image);
			}

			manager->PushUniform();
		},
		[&](VkCommandBuffer _cmdBuff, IUniformManager* _manager, const uint32_t& _frameIndex)
		{
			VK_Resource* lightingResource = m_renderGraph->GetResource("LightingPass");

			VkRenderingAttachmentInfo colorAttachment;
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.pNext = VK_NULL_HANDLE;
			colorAttachment.imageView = lightingResource->image.m_imageView;
			colorAttachment.clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
			colorAttachment.resolveImageView = VK_NULL_HANDLE;
			colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			VkRenderingInfo renderingInfo{};
			renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			renderingInfo.pNext = VK_NULL_HANDLE;
			renderingInfo.renderArea = { { 0, 0 }, *m_renderWindow->GetSwapChain()->GetExtent() };
			renderingInfo.layerCount = 1;
			renderingInfo.viewMask = 0;
			renderingInfo.colorAttachmentCount = 1;
			renderingInfo.pColorAttachments = &colorAttachment;
			renderingInfo.pDepthAttachment = VK_NULL_HANDLE;

			std::vector<VkDescriptorSet> descSets = static_cast<VK_DescriptorManager*>(_manager)->GetDescriptorSets("Lighting", _frameIndex);

			vkCmdBeginRendering(_cmdBuff, &renderingInfo);
			vkCmdBindPipeline(_cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_lightingPipeline->GetPipeline());
			vkCmdSetViewport(_cmdBuff, 0, 1, m_renderWindow->GetWiewport());
			vkCmdSetScissor(_cmdBuff, 0, 1, m_renderWindow->GetScissor());

			VkPushConstantsInfo constantInfo{};
			constantInfo.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
			constantInfo.pNext = VK_NULL_HANDLE;
			constantInfo.layout = *m_lightingPipeline->GetPipelineLayout();
			constantInfo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			constantInfo.offset = 0;
			constantInfo.size = sizeof(Vec3f);
			constantInfo.pValues = m_lightingPipeline->GetPushConstant();

			vkCmdPushConstants2(_cmdBuff, &constantInfo);

			vkCmdDraw(_cmdBuff, 6, 1, 0, 0);

			vkCmdEndRendering(_cmdBuff);
		});

	m_renderGraph->Compile();
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
		ParseEntites(_scene->ecs, &_scene->hierarchy[i], IdentityMat4(), instance, _camera);
	}

	if (m_matricesDatas.empty())
	{
		m_matricesDatas.emplace_back();
	}

	if (m_metaDatas.empty())
	{
		m_metaDatas.emplace_back();
	}

	_uniformManager->StoreData("GBuffer", "matrices",
		m_matricesDatas.data(), ARRAY_SIZE_IN_BYTES(m_matricesDatas), m_frameSync->GetFrameIndex());
	_uniformManager->StoreData("GBuffer", "metaDatas",
		m_metaDatas.data(), ARRAY_SIZE_IN_BYTES(m_metaDatas), m_frameSync->GetFrameIndex());

	_uniformManager->PushUniform();
}

void VK_Render::Draw(Scene* _scene, Camera* _camera, Transform* _cameraTransform, LightManager* _lightManager, IUniformManager* _uniformManager)
{
	LLYN_ASSERT(_scene != nullptr && _uniformManager != nullptr);

	auto [imageIndex, cmdBuff] = m_frameSync->Begin(m_renderWindow->GetSwapChain());

	Update(_scene, _camera, _lightManager, _uniformManager);

	if (!m_indirectCmds.empty())
	{
		memcpy(m_indirectDrawBuff[m_frameSync->GetFrameIndex()]->allocationInfo.pMappedData, m_indirectCmds.data(), ARRAY_SIZE_IN_BYTES(m_indirectCmds));
	}

	m_frameSync->Reset();

	VK_Buffer::BeginCommandBuffer(cmdBuff, 0);

	m_lightingPipeline->SetPushConstant(&_cameraTransform->pos);
	m_renderGraph->Execute(cmdBuff, *VK_Device::Get().GetGraphicsQueue(), _uniformManager, m_frameSync->GetFrameIndex());

	VK_Resource* finalResource = m_renderGraph->GetResource("LightingPass");
	finalResource->image.TransitionLayout(cmdBuff, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VK_DescriptorManager* uniformManager = static_cast<VK_DescriptorManager*>(_uniformManager);
	uniformManager->StoreImage("FrameBuffer", "srcTexture", m_frameSync->GetFrameIndex(), finalResource->image);
	uniformManager->PushUniform();

	m_renderWindow->Clear(cmdBuff, imageIndex);

	VkClearValue colorClear;
	colorClear.color = VkClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f };
	VkClearValue depthValue;
	depthValue.depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

	m_renderWindow->BeginRendering(cmdBuff, imageIndex, &colorClear, &depthValue);

	std::vector<VkDescriptorSet> descSets = uniformManager->GetDescriptorSets("FrameBuffer", m_frameSync->GetFrameIndex());

	vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_frameBufferPipeline->GetPipeline());
	vkCmdSetViewport(cmdBuff, 0, 1, m_renderWindow->GetWiewport());
	vkCmdSetScissor(cmdBuff, 0, 1, m_renderWindow->GetScissor());
	vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_frameBufferPipeline->GetPipelineLayout(), 0,
		static_cast<uint32_t>(descSets.size()), descSets.data(), 0, VK_NULL_HANDLE);
	vkCmdDraw(cmdBuff, 6, 1, 0, 0);
	m_renderWindow->EndRendering(cmdBuff);

	vkEndCommandBuffer(cmdBuff);


	/*	std::vector<VkDescriptorSet> descSets =
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
		}*/

	VkCommandBuffer imGuiCmdBuff = m_imgui->Draw(m_renderWindow, m_frameSync->GetFrameIndex(), imageIndex);

	m_renderWindow->Display(imGuiCmdBuff, imageIndex);

	vkEndCommandBuffer(imGuiCmdBuff);

	std::vector<VkCommandBuffer> cmdBuffList = { cmdBuff, imGuiCmdBuff };

	m_frameSync->End(m_renderWindow->GetSwapChain(), imageIndex, cmdBuffList);
}

void VK_Render::AllocateVariable()
{
	m_instance = new VK_Instance;
	m_renderWindow = new VK_RenderWindow;
	m_frameSync = new VK_FrameSync;
	m_gBufferPipeline = new VK_Pipeline;
	m_lightingPipeline = new VK_Pipeline;
	m_frameBufferPipeline = new VK_Pipeline;
	m_depthResources = new VK_DepthResources;
	m_renderGraph = new VK_RenderGraph;

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

void VK_Render::ParseEntites(ECS* _ecs, const Entity* _entity, Mat4 _transformMat,
	uint32_t& _instance, const Camera* _camera)
{
	LLYN_ASSERT(_ecs != nullptr || _entity != nullptr);

	if (_entity->id == TOMBSTONE_ENTITY)
	{
		return;
	}

	const Transform* baseTransform = _ecs->GetComponent<Transform>(_entity->id);
	_transformMat *= baseTransform->GetMatrix();

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

		const Mat4 transfromMatrix = _transformMat;
		const Mat4 MVP = (_camera->GetProjMatrix() * _camera->GetViewMatrix()) * transfromMatrix;

		MatricesData matrices;
		matrices.mvp = MVP;
		matrices.localPos = transfromMatrix;
		matrices.normal = TransposeMat4(InverseMat4(transfromMatrix));

		m_matricesDatas.emplace_back(matrices);

		_instance += 1;
	}

	for (int i = 0; i < _entity->childs.size(); ++i)
	{
		ParseEntites(_ecs, &_entity->childs[i], _transformMat, _instance, _camera);
	}
}