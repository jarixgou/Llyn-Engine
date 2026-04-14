#ifndef POINT_LIGHT__H
#define POINT_LIGHT__H

#include "Component.h"

namespace Llyn
{
	struct PointLightData;

	class PointLight : public Component
	{
	private:
		uint16_t m_lightId;
		PointLightData* m_data;
	public:
		PointLight();
		~PointLight() override;

		void Draw(Camera* _camera, Transform* _transform) override;

		glm::vec3 GetPosition() const;
		void SetPosition(glm::vec3 _newPosition) const;

		glm::vec3 GetAmbient() const;
		void SetAmbient(glm::vec3 _newAmbient) const;

		glm::vec3 GetDiffuse() const;
		void SetDiffuse(glm::vec3 _newDiffuse) const;

		glm::vec3 GetSpecular() const;
		void SetSpecular(glm::vec3 _newSpecular) const;

		float GetConstant() const;
		void SetConstant(float _newConstant) const;

		float GetLinear() const;
		void SetLinear(float _newLinear) const;

		float GetQuadratic() const;
		void SetQuadratic(float _newQuadratic) const;
	};
}

#endif