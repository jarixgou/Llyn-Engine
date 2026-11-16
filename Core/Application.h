#ifndef APPLICATION__H
#define APPLICATION__H

class Editor;
class Engine;

class Application
{
private:
	Engine* m_engine = nullptr;
	Editor* m_editor = nullptr;
public:
	void Run();

	void SetFrameBufferResized(bool _resized);
private:
	void Init();
	void MainLoop();
	void Cleanup();
};

#endif