#include "Component.h"

#include <glm/mat4x4.hpp>
#include "../IDHelper/IDHelper.h"

namespace Llyn
{
	Component::Component()
	{
		m_id = GetNextID();
	}

	size_t Component::GetID()
	{
		return m_id;
	}

	void Component::SetID(size_t _id)
	{
		m_id = _id;
	}
}
