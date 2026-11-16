#include "Application.h"

#include <iostream>

#include "CompileShader.h"
#include "Render/Window.h"
#include "Engine.h"
#include "Editor/Editor.h"

#include "Vector/Vec2.h"

void Application::Run()
{
	Init();
	MainLoop();
	Cleanup();
}

void Application::Init()
{
	srand(time(NULL));

	Window::Get().Init({ 1920, 1080 }, "Llyn");

	CompileShaders();

	m_engine = new Engine;
	m_engine->Init();

#ifndef _NEDITOR
	m_editor = new Editor;
	m_editor->Init(m_engine);
#endif
}

void Application::MainLoop()
{
	while (!Window::Get().GetWindowClosed())
	{
		Window::Get().PollEvents();

#ifndef _NEDITOR
		m_editor->Update();
#endif

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