#include "Timer.h"

Timer::Timer(std::chrono::duration<long long> _dueDuration)
{
	start = std::chrono::high_resolution_clock::now();
	dueDuration = _dueDuration;
}

bool Timer::IsDueDuration()
{
	end = std::chrono::high_resolution_clock::now();
	return (dueDuration <= std::chrono::duration_cast<std::chrono::seconds>(end - start));
}