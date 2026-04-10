#ifndef DIRECT_LIGHT__H
#define DIRECT_LIGHT__H
#include "Component.h"

namespace Llyn
{
	struct DirectLightData;

	class DirectLight : public Component
	{
	private:
		DirectLightData* m_data;

	public:
	};
}

#endif