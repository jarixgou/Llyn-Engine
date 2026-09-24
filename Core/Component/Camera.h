#ifndef CAMERA__H
#define CAMERA__H

#include "Component.h"
#include "../Vector/Vec2.h"
#include "../Vector/Vec3.h"
#include "../Math/Matrix/Mat4.h"

struct Transform;

class Camera : public IComponent
{
private:
	Mat4 m_view;
	Mat4 m_proj;
	Vec3f m_up = { 0.0f, 1.0f, 0.0f };
	Vec2f m_size = { 1920.0f, 1080.0f };
	float m_fov = 60.0f;
	float m_near = 0.1f;
	float m_far = 1000.0f;

	float m_speed = 30.0f;
	float m_sensitivity = 30.0f;

	float m_pitch = 0.0f;

	bool m_firstClick = true;
public:
	void Update(Transform* _transform, const float& _dt);

	void UpdateMatrices(const Transform& _transform);

	Mat4 GetViewMatrix() const;
	Mat4 GetProjMatrix() const; 

	void SetUp(const Vec3f& _newUp);
	Vec3f GetUp() const;

	void SetSize(const Vec2f& _newSize);
	Vec2f GetSize() const;

	void SetFOV(const float& _newFOV);
	float GetFOV() const;

	void SetNear(const float& _newNear);
	float GetNear() const;

	void SetFar(const float& _newFar);
	float GetFar();
};

#endif