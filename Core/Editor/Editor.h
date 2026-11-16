#ifndef EDITOR__H
#define EDITOR__H

class Engine;

class Editor
{
private:
	Engine* m_engine = nullptr;
public:
	void Init(Engine* _engine);
	void Update();
	void Cleanup();
};

#endif