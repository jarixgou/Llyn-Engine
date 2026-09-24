#include "Window.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../Vector/Vec2.h"

Window& Window::Get()
{
	static Window instance;
	return instance;
}

void Window::Init(const Vec2i& _size, const std::string& _name)
{
	m_size = new Vec2i;
	*m_size = _size;

	m_name = _name;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(m_size->x, m_size->y, m_name.c_str(), nullptr, nullptr);
}

void Window::PollEvents() const
{
	if (m_window == nullptr)
	{
		return;
	}

	glfwPollEvents();
}

bool Window::GetWindowClosed() const
{
	if (m_window == nullptr)
	{
		return true;
	}

	return glfwWindowShouldClose(m_window);
}

Vec2i Window::GetSize()
{
	return *m_size;
}

GLFWwindow* Window::GetGLFWWindow()
{
	return m_window;
}
