#include "Camera.h"



#include "../Shader/Shader.h"

namespace ENGINE_NAME
{
	Camera::Camera(const glm::vec3& _position, const glm::vec2& _size)
	{
		m_position = _position;
		m_orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);

		m_size = _size;

		m_fov = 60.f;
		m_nearPlane = 0.1f;
		m_farPlane = 100.f;

		m_speed = 0.1f;
		m_sensitivity = 100.f;

		m_firstClick = true;
	}

	Camera::~Camera()
	{
	}

	void Camera::Input(GLFWwindow* _window)
	{
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_position += m_speed * m_orientation;
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_position += m_speed * -glm::normalize(glm::cross(m_orientation, m_up));
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_position += m_speed * -m_orientation;
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_position += m_speed * glm::normalize(glm::cross(m_orientation, m_up));
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			m_position += m_speed * m_up;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			m_position += m_speed * -m_up;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			m_speed = 0.4f;
		}
		else if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			m_speed = 0.1f;
		}

		if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			if (m_firstClick)
			{
				glfwSetCursorPos(_window, m_size.x / 2.f, m_size.y / 2.f);
				m_firstClick = false;
			}

			double mouseX;
			double mouseY;
			glfwGetCursorPos(_window, &mouseX, &mouseY);

			float rotX = m_sensitivity * (mouseY - (m_size.y / 2.f)) / m_size.y;
			float rotY = m_sensitivity * (mouseX - (m_size.x / 2.f)) / m_size.x;

			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-rotX), glm::normalize(glm::cross(m_orientation, m_up)));
			glm::vec3 newOrientation = glm::vec3(rotationMatrix * glm::vec4(m_orientation, 0.0f));

			if (abs(glm::angle(newOrientation, m_up) - glm::radians(90.f)) <= glm::radians(85.f))
			{
				m_orientation = newOrientation;
			}

			m_orientation = glm::rotate(m_orientation, glm::radians(-rotY), m_up);

			glfwSetCursorPos(_window, m_size.x / 2.f, m_size.y / 2.f);
		}
		else if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_firstClick = true;
		}
	}

	void Camera::UpdateMatrix()
	{
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		view = glm::lookAt(m_position, m_position + m_orientation, m_up);
		proj = glm::perspective(glm::radians(m_fov), m_size.x / m_size.y, m_nearPlane, m_farPlane);
		m_camMatrix = proj * view;
	}

	void Camera::Matrix(Shader& _shader)
	{

		_shader.SetUniform("uCamMatrix", glm::value_ptr(m_camMatrix), 1);
	}

	void Camera::SetPositon(const glm::vec3& _position)
	{
		m_position = _position;
	}

	const glm::vec3& Camera::GetPosition() const
	{
		return m_position;
	}

	void Camera::SetFov(const float& _fov)
	{
		m_fov = _fov;
	}

	const float& Camera::GetFov() const
	{
		return m_fov;
	}

	void Camera::SetNearPlane(const float& _nearPlane)
	{
		m_nearPlane = _nearPlane;
	}

	const float& Camera::GetNearPlane() const
	{
		return m_nearPlane;
	}

	void Camera::SetFarPlane(const float& _farPlane)
	{
		m_farPlane = _farPlane;
	}

	const float& Camera::GetFarPlane() const
	{
		return m_farPlane;
	}
}
