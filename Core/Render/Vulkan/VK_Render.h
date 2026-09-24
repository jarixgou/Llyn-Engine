#ifndef VK_RENDER__H
#define VK_RENDER__H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "../Render.h"

#include "../MatricesData.h"
#include "../MetaData.h"

class VK_Render : public IRender
{
private:
	VK_Instance* m_instance = nullptr;
	VK_Device* m_device = nullptr;
	VK_FrameSync* m_frameSync = nullptr;
	VK_Pipeline* m_defaultPipeline = nullptr;
	VK_DepthResources* m_depthResources = nullptr;

	VK_RenderWindow* m_renderWindow = nullptr;

	std::vector<VK_Buffer*> m_indirectDrawBuff;

	VK_ImGui* m_imgui = nullptr;

	std::vector<VkDrawIndirectCommand> m_indirectCmds;
	std::vector<MetaData> m_metaDatas;
	std::vector<MatricesData> m_matricesDatas;
public:
	VK_Render() = default;
	~VK_Render() override;

	void Init(IUniformManager** _uniformManger) override;
	void Update(Scene* _scene, Camera* _camera, LightManager* _lightManager, IUniformManager* _uniformManager) override;
	void Draw(Scene* _scene, Camera* _camera, Transform* _cameraTransform, LightManager* _lightManager, IUniformManager* _uniformManager) override;
	void Cleanup() override;

private:
	void AllocateVariable();

	void ClearWindow(IUniformManager* _uniformManager, uint32_t _imageIndex);
	void DisplayWindow(uint32_t _imageIndex);

	void ParseEntites(ECS* _ecs, const Entity* _entity, Transform _currentTransform, uint32_t&
	                  _instance, const Camera* _camera);
};

#endif