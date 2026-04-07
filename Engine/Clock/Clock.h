#ifndef CLOCK__H
#define CLOCK__H
#include <chrono>

namespace Llyn
{
	class Clock
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	public:
		Clock();
		~Clock() = default;

		float Restart();
	};
}

#endif