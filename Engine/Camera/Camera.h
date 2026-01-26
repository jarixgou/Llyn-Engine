#ifndef CAMERA__H
#define CAMERA__H

#include <glad/glad.h>      
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Camera.h"
#include "Camera.h"
#include "../Macro.h"

namespace ENGINE_NAME
{
	class Shader;

	class Camera
	{
	private:
		glm::vec3 m_position;
		glm::vec3 m_orientation;
		glm::vec3 m_up;
		
		glm::vec2 m_size;

		float m_fov;
		float m_nearPlane;
		float m_farPlane;

		Shader* m_shader;

		float m_speed;
		float m_sensitivity;

		bool m_firstClick;
	public:
		Camera(const glm::vec3& _position, const glm::vec2& _size, Shader* _shader);
		~Camera();

		void Update(GLFWwindow* _window);

		void SetPositon(const glm::vec3& _position);
		const glm::vec3& GetPosition() const;

		void SetFov(const float& _fov);
		const float& GetFov() const;

		void SetNearPlane(const float& _nearPlane);
		const float& GetNearPlane() const;

		void SetFarPlane(const float& _farPlane);
		const float& GetFarPlane() const;
	private:
		void Matrix();
	};
}

#endif