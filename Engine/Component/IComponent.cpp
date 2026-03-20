#include "IComponent.h"

#include <glm/mat4x4.hpp>
#include "../IDHelper/IDHelper.h"

namespace Llyn
{
	IComponent::IComponent()
	{
		m_id = GetNextID();
	}

	size_t IComponent::GetID()
	{
		return m_id;
	}

	void IComponent::SetID(size_t _id)
	{
		m_id = _id;
	}
}
