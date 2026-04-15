#ifndef SPOT_LIGHT__H
#define SPOT_LIGHT__H
#include "Component.h"

namespace Llyn
{
	struct SpotLightData;

	class SpotLight : public Component
	{
	private:
		uint16_t m_lightId;
		SpotLightData* m_data;
	public:
		SpotLight();
		~SpotLight() override;

		void Draw(Camera* _camera, Transform* _transform) override;

		glm::vec3 GetPosition();
		void SetPosition(glm::vec3 _newPosition);

		glm::vec3 GetDirection();
		void SetDirection(glm::vec3 _newDirection);

		glm::vec3 GetAmbient();
		void SetAmbient(glm::vec3 _newAmbient);

		glm::vec3 GetDiffuse();
		void SetDiffuse(glm::vec3 _newDiffuse);

		glm::vec3 GetSpecular();
		void SetSpecular(glm::vec3 _newSpecular);

		float GetCutOff();
		void SetCutOff(float _newCutOff);

		float GetOuterCutOff();
		void SetOuterCutOff(float _newOuterCutOff);
	};
}

#endif             