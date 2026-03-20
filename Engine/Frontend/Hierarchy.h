#ifndef HIERARCHY__H
#define HIERARCHY__H
#include <vector>

namespace Llyn
{
	class GameObject;

	class Hierarchy
	{
	private:
		std::vector<GameObject*>* m_goList;
		GameObject* m_selectedGo;

		static Hierarchy* s_instance;
	public:
		Hierarchy();
		~Hierarchy();

		static Hierarchy* Get();

		void Draw();

		void SetGameObjectList(std::vector<GameObject*>* _goList);

		GameObject* GetSelectedGameObject() const;
		GameObject** GetSelectedGameObjectPtr();
	private:
		void DrawTree(GameObject* _rootNode);
		bool DrawTreeNode(GameObject* _go);
	};
}

#endif