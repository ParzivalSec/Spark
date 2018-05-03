#pragma once

typedef __int64 Cycles;

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