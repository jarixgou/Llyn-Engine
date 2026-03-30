#ifndef TRANSFORM__H
#define TRANSFORM__H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/detail/type_quat.hpp>

#include "Component.h"

namespace Llyn
{
	struct Transform : public Component
	{
		glm::quat rotation;
		glm::vec3 position;
		glm::vec3 scale;

		Transform();
		~Transform() override;

		glm::mat4 GetMatrix();

		void Draw(Camera* _camera, glm::mat4 _model) override;
	};
}

#endif