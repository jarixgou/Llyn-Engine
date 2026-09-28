#ifndef MATRICES_DATA__H
#define MATRICES_DATA__H

#include "../Math/Matrix/Mat4.h"
#include "../Math/Matrix/Mat3.h"

struct MatricesData
{
	Mat4 mvp;
	Mat4 localPos;
	Mat4 normal;
};

#endif // !MATRICES_DATA__H