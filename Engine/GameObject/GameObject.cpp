#include "GameObject.h"

#include <glm/gtc/type_ptr.hpp>

#include "../IDHelper/IDHelper.h"
#include "../Camera/Camera.h"
#include "../Component/Transform.h"
#include "../Memory/MemoryHelper.h"
#include "../Render/OpenGL/Shader/Shader.h"

namespace Llyn
{
	GameObject::GameObject()
	{
		ALLOCATE_MEMORY(m_transform);

		m_name = "GameObject";
		m_tag = "Default";

		m_parent = nullptr;

		m_id = GetNextID();

		m_drawable = true;
	}

	GameObject::GameObject(std::string _name)
	{
		ALLOCATE_MEMORY(m_transform);

		m_name = _name;
		m_tag = "Default";

		m_parent = nullptr;

		m_drawable = true;
	}

	GameObject::~GameObject()
	{
		DELETE_MEMORY(m_transform);

		m_components.clear();

		for (auto& child : m_childs)
		{
			DELETE_MEMORY(child);
		}
		m_childs.clear();
	}

	void GameObject::Update()
	{
	}

	void GameObject::Draw(Camera* _camera, Transform* _transform)
	{

		glm::mat4 model = _transform * m_transform->GetMatrix();

		for (auto& component : m_components)
		{
			component->Draw(_camera, model);
		}
		for (auto& child : m_childs)
		{
			child->Draw(_camera, model);
		}
	}

	std::string GameObject::GetName()
	{
		return m_name;
	}

	void GameObject::SetName(const char* _name)
	{
		m_name = _name;
	}

	Transform* GameObject::GetTransform()
	{
		return m_transform;
	}

	void GameObject::AddComponent(Component* _component)
	{
		m_components.emplace_back(_component);
	}

	std::vector<Component*>* GameObject::GetComponents()
	{
		return &m_components;
	}

	std::vector<GameObject*> GameObject::GetChild()
	{
		return m_childs;
	}

	size_t GameObject::GetChildCount()
	{
		return m_childs.size();
	}

	size_t GameObject::GetID()
	{
		return m_id;
	}
}
