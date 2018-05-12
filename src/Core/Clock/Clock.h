#pragma once

typedef __int64 Cycles;

/**
 * HighPrecisionClock is a utility to get high resolution time-stamps.
 * Internally the clock is built upon the QueeryPerformanceCounter() API
 * of the WinAPI. Combined with the frequency it is possible to get
 * measurements in microseconds.
 */
class HighPrecisionClock
{
public:
	HighPrecisionClock();

	void Start(void);
	double Get(void) const;

private:
	double m_frequency;
	Cycles m_start;
};