#include "Scenarios/Scenarios.h"

#include <iostream>
#include "Clock/Clock.h"

int main(int argc, char** argv)
{
	if (argc <= 1 || argc > 2)
	{
		std::cerr << "Usage: bench_spark++.exe SCENARIO_ID" << std::endl;
		return -1;
	}

	HighPrecisionClock perf_clock;

	perf_clock.Start();
	scenarios[atoi(argv[1])]();
	std::cout << perf_clock.Get() << std::endl;
}
