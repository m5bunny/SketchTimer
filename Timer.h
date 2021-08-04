#ifndef  TIMER_H_
#define TIMER_H_
#include <chrono>

class Timer
{
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<long long> dueDuration;

public:
	Timer(std::chrono::duration<long long> _dueDuration = std::chrono::seconds(1));
	bool IsDueDuration();
};

#endif // ! TIMER_H_