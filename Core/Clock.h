#ifndef CLOCK__H
#define CLOCK__H
#include <chrono>

class Clock
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
public:
	float Restart();
};

#endif