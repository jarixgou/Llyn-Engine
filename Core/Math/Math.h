#ifndef MATH__H
#define MATH__H

#include "../Vector/FwdVec2.h"
#include "../Vector/FwdVec3.h"
#include "../Vector/FwdVec4.h"

struct Mat2;
struct Mat3;
struct Mat4;

class Quaternions;

Quaternions LookAtQuat(const Vec3f& _forward, Vec3f& _up);

Mat4 LookAtMat(const Vec3f& _forward, const Vec3f& _up);
Mat4 View(const Vec3f& _eye, const Vec3f& _target, const Vec3f& _worldUp);

Mat4 Perspective(const float& _fov, const float& _aspectRatio, const float& _near, const float& _far);

Mat4 Translate(const Mat4& _mat, const Vec3f& _vec);
Mat4 Scale(const Mat4& _mat, const Vec3f& _vec);

Mat4 IdentityMat4();
Mat3 IdentityMat3();
Mat2 IdentityMat2();

Mat4 TransposeMat4(const Mat4 _m);

Mat3 TransposeMat3(const Mat3& _mat);
float DeterminantMat3(const Mat3& _mat);
Mat3 CofactorSignsMat3(const Mat3& _mat);
Mat3 MinorMat3(const Mat3& _mat);
Mat3 InverseMat3(const Mat3& _mat);

Vec3f RotateAroundAxis(Vec3f _v, Vec3f _axis, float _angle);
Vec3f Cross(const Vec3f& _vA, const Vec3f& _vB);

Vec4f Normalize(const Vec4f& _vec);
Vec3f Normalize(const Vec3f& _vec);
Vec2f Normalize(const Vec2f& _vec);

float Dot(const Vec4f& _vA, const Vec4f& _vB);
float Dot(const Vec3f& _vA, const Vec3f& _vB);
float Dot(const Vec2f& _vA, const Vec2f& _vB);

float Angle(const Vec3f& _vA, const Vec3f& _vB);

float Length(const Vec4f& _vec);
float Length(const Vec3f& _vec);
float Length(const Vec2f& _vec);

Vec4f DegToRad(const Vec4f& _vec);
Vec3f DegToRad(const Vec3f& _vec);
Vec2f DegToRad(const Vec2f& _vec);
float DegToRad(const float& _a);

Vec4f RadToDeg(const Vec4f& _vec);
Vec3f RadToDeg(const Vec3f& _vec);
Vec2f RadToDeg(const Vec2f& _vec);
float RadToDeg(const float& _a);

#endif