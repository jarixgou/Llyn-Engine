#include "Vertex.h"

bool Vertex::operator==(const Vertex& _v)
{
	return pos == _v.pos && normal == _v.normal && tangent == _v.tangent && bitangent == _v.bitangent &&
		color == _v.color && uv0 == _v.uv0 && uv1 == _v.uv1;
}

bool Vertex::operator!=(const Vertex& _v)
{
	return !(*this == _v);
}