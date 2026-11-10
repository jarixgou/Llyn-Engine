#ifndef MATH__H
#define MATH__H

template<typename T>
T Clamp(const T& _v, const T& _a, const T& _b)
{
	if (_v < _a)
	{
		return _a;
	}
	else if (_v > _b)
	{
		return _b;
	}

	return _v;
}

#endif