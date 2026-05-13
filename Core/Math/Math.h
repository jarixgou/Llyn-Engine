#ifndef MATH__H
#define MATH__H

#include "../Vector/FwdVec2.h"
#include "../Vector/FwdVec3.h"
#include "../Vector/FwdVec4.h"

class Mat2;
class Mat3;
class Mat4;

Mat4 LookAt(const Vec3f& _eye, const Vec3f& _target, const Vec3f& _worldUp);

Mat4 Perspective(const float& _fov, const float& _aspectRatio, const float& _near, const float& _far);

Mat4 Translate(const Vec3f& _vec);
Mat4 Scale(const Vec3f& _vec);

Mat4 IdentityMat4();
Mat3 IdentityMat3();
Mat2 IdentityMat2();

Vec3f Cross(const Vec3f& _vA, const Vec3f& _vB);

Vec4f Normalize(const Vec4f& _vec);
Vec3f Normalize(const Vec3f& _vec);
Vec2f Normalize(const Vec2f& _vec);

float Dot(const Vec4f& _vA, const Vec4f& _vB);
float Dot(const Vec3f& _vA, const Vec3f& _vB);
float Dot(const Vec2f& _vA, const Vec2f& _vB);

float Length(const Vec4f& _vec);
float Length(const Vec3f& _vec);
float Length(const Vec2f& _vec);

#endif