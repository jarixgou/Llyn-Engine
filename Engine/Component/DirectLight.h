#ifndef DIRECT_LIGHT__H
#define DIRECT_LIGHT__H
#include "Component.h"

namespace Llyn
{
	struct DirectLightData;

	struct Transform;
	class Camera;

	class DirectLight : public Component
	{
	private:
		uint16_t m_lightId;
		DirectLightData* m_data;
	public:
		DirectLight();
		~DirectLight() override;

		void Draw(Camera* _camera, Transform* _transform) override;

		glm::vec3 GetDirection();
		void SetDir(glm::vec3 _newDir);

		glm::vec3 GetAmbient();
		void SetAmbient(glm::vec3 _newAmbient);

		glm::vec3 GetDiffuse();
		void SetDiffuse(glm::vec3 _newDiffuse);

		glm::vec3 GetSpecular();
		void SetSpecular(glm::vec3 _newSpecular);
	};
}

#endif