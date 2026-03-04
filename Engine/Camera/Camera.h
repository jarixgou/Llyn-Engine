#ifndef CAMERA__H
#define CAMERA__H

#include <glad/glad.h>      
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/fwd.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Llyn
{
	class Shader;

	class Camera
	{
	private:
		glm::vec3 m_position;
		glm::vec3 m_orientation;
		glm::vec3 m_up;
		
		glm::mat4 m_camMatrix;

		glm::vec2 m_size;

		float m_fov;
		float m_nearPlane;
		float m_farPlane;

		float m_speed;
		float m_sensitivity;

		bool m_firstClick;
	public:
		Camera(const glm::vec3& _position, const glm::vec2& _size);
		~Camera();

		void Input(GLFWwindow* _window, float _dt);
		void UpdateMatrix();
		void Matrix(Shader& _shader);

		void SetPositon(const glm::vec3& _position);
		const glm::vec3& GetPosition() const;

		void SetFov(const float& _fov);
		const float& GetFov() const;

		void SetNearPlane(const float& _nearPlane);
		const float& GetNearPlane() const;

		void SetFarPlane(const float& _farPlane);
		const float& GetFarPlane() const;
	private:
	};
}

#endif