#include "Clock.h"

namespace Llyn
{
	Clock::Clock()
	{
		Restart();
	}

	float Clock::Restart()
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = now - m_start;
		m_start = now;
		return duration.count();
	}
}
