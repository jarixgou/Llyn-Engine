#ifndef MATRICES_DATA__H
#define MATRICES_DATA__H

#include "../Math/Matrix/Mat4.h"
#include "../Math/Matrix/Mat3.h"

struct MatricesData
{
	Mat4 mvp;
	Mat3 normal;
	float pad[3];
};

#endif // !MATRICES_DATA__H