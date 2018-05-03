#include "Clock.h"
#include "Windows.h"

HighPrecisionClock::HighPrecisionClock()
	: m_start(0)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	m_frequency = 1.0 / static_cast<double>(frequency.QuadPart / 1000000.0);
}

void HighPrecisionClock::Start()
{
	LARGE_INTEGER cycles;
	QueryPerformanceCounter(&cycles);
	m_start = cycles.QuadPart;
}

double HighPrecisionClock::Get() const
{
	LARGE_INTEGER curr_cycles;
	QueryPerformanceCounter(&curr_cycles);
	return (static_cast<double>(curr_cycles.QuadPart) - m_start) * m_frequency;
}
