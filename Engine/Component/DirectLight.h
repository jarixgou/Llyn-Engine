#ifndef DIRECT_LIGHT__H
#define DIRECT_LIGHT__H
#include "Component.h"

namespace Llyn
{
	struct DirectLightData;

	class DirectLight : public Component
	{
	private:
		uint16_t m_lightId;
		DirectLightData* m_data;
	public:

		void Draw(Camera* _camera, Transform* _transform) override;
	};
}

#endif