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

	struct CameraData
	{
		glm::mat4 view = glm::mat4(1.f);
		glm::mat4 proj = glm::mat4(1.f);

		glm::vec3 position = { 0.f, 0.f, 0.f };
	};

	class Camera
	{
	private:
		CameraData m_cameraData;
		glm::vec3 m_orientation;
		glm::vec3 m_up;

		glm::vec2 m_size;

		float m_fov;
		float m_nearPlane;
		float m_farPlane;

		float m_speed;
		float m_sensitivity;
		
		bool m_firstClick;

		unsigned int m_ubo;

	public:
		Camera(const glm::vec3& _position, const glm::vec2& _size);
		~Camera();

		void Input(GLFWwindow* _window, float _dt);
		void Update();

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