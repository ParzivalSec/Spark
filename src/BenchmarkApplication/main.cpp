#include "Scenarios/Scenarios.h"

#include <iostream>

int main(int argc, char** argv)
{
	std::cout << argc << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i] << std::endl;
	}

	if (argc <= 1 || argc > 2)
	{
		std::cerr << "Usage: bench_spark++.exe SCENARIO_ID" << std::endl;
		return -1;
	}

	scenarios[atoi(argv[1])]();
}
