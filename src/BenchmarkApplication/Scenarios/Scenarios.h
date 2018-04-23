#pragma once

// Include the declarations of all scenario functions
#include "MemorySystem/LinearAllocator_Benchmarks.h"

typedef void(*BenchmarkScenarioFunction)();

const BenchmarkScenarioFunction scenarios[] =
{
	&allocate_100KB_unaligned_hundred_times,		// ID 0
	&allocate_1MB_unaligned_hundred_times,			// ID 1
	&allocate_50MB_unaligned_ten_times,				// ID 2
};