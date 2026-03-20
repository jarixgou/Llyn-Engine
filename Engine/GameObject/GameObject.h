#ifndef GAME_OBJECT__H
#define GAME_OBJECT__H

#include <string>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>

namespace Llyn
{
	class IComponent;
	struct Transform;
	class Camera;

	class GameObject
	{
	private:
		Transform* m_transform;

		std::string m_name;
		std::string m_tag;

		std::vector<IComponent*> m_components;

		std::vector<GameObject*> m_childs;

		GameObject* m_parent;

		size_t m_id;

		bool m_drawable;
	public:
		GameObject();
		GameObject(std::string _name);
		~GameObject();

		void Update();
		void Draw(Camera* _camera, glm::mat4 _model = glm::mat4(1.f));

		std::string GetName();
		void SetName(const char* _name);

		Transform* GetTransform();

		void AddComponent(IComponent* _component);

		std::vector<IComponent*>* GetComponents();

		std::vector<GameObject*> GetChild();
		size_t GetChildCount();
		
		size_t GetID();
	};
}

#endif