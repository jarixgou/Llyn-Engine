#include "Camera.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <valarray>
#include <GLFW/glfw3.h>
#include "../Render/Window.h"

#include "Transform.h"
#include "../Math/Math.h"

void Camera::Update(Transform* _transform, const float& _dt)
{
	GLFWwindow* window = Window::Get().GetGLFWWindow();

	Quaternions quat = _transform->rot;
	Vec3f forward = quat.GetForward();
	Vec3f right = quat.GetRight();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		_transform->pos += {forward.x * m_speed* _dt, forward.y * m_speed * _dt, forward.z * m_speed * _dt};
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		_transform->pos += {-forward.x* m_speed* _dt, -forward.y* m_speed* _dt, -forward.z* m_speed* _dt};
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		_transform->pos += {right.x* m_speed* _dt, right.y* m_speed* _dt, right.z* m_speed* _dt};
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		_transform->pos += {-right.x* m_speed* _dt, -right.y* m_speed* _dt, -right.z* m_speed* _dt};
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_speed = 50.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		m_speed = 30.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		_transform->pos += {-m_up.x * m_speed * _dt, -m_up.y * m_speed * _dt, -m_up.z * m_speed * _dt};
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		_transform->pos += {m_up.x* m_speed* _dt, m_up.y* m_speed* _dt, m_up.z* m_speed* _dt};
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (m_firstClick)
		{
			glfwSetCursorPos(window, m_size.x / 2.0f, m_size.y / 2.0f);
			m_firstClick = false;
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = m_sensitivity * (mouseY - (m_size.y / 2.0f)) / m_size.y;
		float rotY = m_sensitivity * (mouseX - (m_size.x / 2.0f)) / m_size.x;

		float newPitch = std::clamp(m_pitch - rotX, -89.0f, 89.0f);
		float appliedRotX = m_pitch - newPitch;
		m_pitch = newPitch;

		Quaternions qYaw;
		qYaw.SetAxisAngle({ 0.0f, 1.0f, 0.0f }, DegToRad(rotY));

		Quaternions qPitch;
		qPitch.SetAxisAngle({ 1.0f, 0.0f, 0.0f }, DegToRad(-appliedRotX));

		_transform->rot = qYaw * _transform->rot * qPitch;
		_transform->rot.Normalize();

		glfwSetCursorPos(window, m_size.x / 2.0f, m_size.y / 2.0f);

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_firstClick = true;
	}

	UpdateMatrices(*_transform);
}

void Camera::UpdateMatrices(const Transform& _transform)
{
	m_proj = Perspective(DegToRad(m_fov), m_size.x / m_size.y, m_near, m_far);

	Vec3f forward = _transform.rot.GetForward();

	Vec3f target = {
		_transform.pos.x + forward.x,
		_transform.pos.y + forward.y,
		_transform.pos.z + forward.z
	};

	m_view = View(_transform.pos, target, m_up);
}

Mat4 Camera::GetViewMatrix() const
{
	return m_view;
}

Mat4 Camera::GetProjMatrix() const
{
	return m_proj;
}

void Camera::SetUp(const Vec3f& _newUp)
{
	m_up = _newUp;
}

Vec3f Camera::GetUp() const
{
	return m_up;
}

void Camera::SetSize(const Vec2f& _newSize)
{
	m_size = _newSize;
}

Vec2f Camera::GetSize() const
{
	return m_size;
}

void Camera::SetFOV(const float& _newFOV)
{
	m_fov = _newFOV;
}

float Camera::GetFOV() const
{
	return m_fov;
}

void Camera::SetNear(const float& _newNear)
{
	m_near = _newNear;
}

float Camera::GetNear() const
{
	return m_near;
}

void Camera::SetFar(const float& _newFar)
{
	m_far = _newFar;
}

float Camera::GetFar()
{
	return m_far;
}
