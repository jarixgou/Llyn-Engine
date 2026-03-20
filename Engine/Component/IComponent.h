#ifndef ICOMPONENT__H
#define ICOMPONENT__H
#include <glm/fwd.hpp>

namespace Llyn
{
	class Camera;

	class IComponent
	{
	private:
		size_t m_id;
	public:
		IComponent();
		virtual ~IComponent() = default;

		virtual void Draw(Camera* _camera, glm::mat4 _model) = 0;

		size_t GetID();
		void SetID(size_t _id);
	};
}

#endif