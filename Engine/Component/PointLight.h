#ifndef POINT_LIGHT__H
#define POINT_LIGHT__H

#include "Component.h"

namespace Llyn
{
	struct PointLightData;

	class PointLight : public Component
	{
	private:
		PointLightData* m_data;
	public:

	};
}

#endif