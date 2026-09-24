#include "Application.h"

#include <iostream>

#include "Render/Window.h"
#include "Engine.h"

void Application::Run()
{
	Init();
	MainLoop();
	Cleanup();
}

void Application::Init()
{
	Window::Get().Init({ 1920, 1080 }, "Llyn");

	m_engine = new Engine;
	m_engine->Init();
}

void Application::MainLoop()
{
	while (!Window::Get().GetWindowClosed())
	{
		Window::Get().PollEvents();

		m_engine->Update();
	}
}

void Application::Cleanup()
{
	m_engine->Cleanup();
	delete m_engine;
	m_engine = nullptr;

	/*CleanupSwapChain();

	glfwDestroyWindow(m_window);

	glfwTerminate();*/
}