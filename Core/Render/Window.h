#ifndef WINDOW__H
#define WINDOW__H

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <string>

#include "../Vector/FwdVec2.h"

struct GLFWwindow;

class Window
{
private:
	Window() = default;

	GLFWwindow* m_window = nullptr;
	Vec2i* m_size = nullptr;
	std::string m_name = "Default";
public:
	static Window& Get();

	void Init(const Vec2i& _size, const std::string& _name);

	void PollEvents() const;
	bool GetWindowClosed() const;
	Vec2i GetSize();

	GLFWwindow* GetGLFWWindow();
};

#endif