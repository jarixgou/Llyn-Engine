#ifndef APPLICATION__H
#define APPLICATION__H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
#include <vulkan/vulkan_raii.hpp>

#include "Component/Transform.h"
#include "Component/Camera.h"

class Engine;

class Application
{
private:
	Engine* m_engine = nullptr;
public:
	void Run();

	void SetFrameBufferResized(bool _resized);
private:
	void Init();
	void MainLoop();
	void Cleanup();
};

#endif