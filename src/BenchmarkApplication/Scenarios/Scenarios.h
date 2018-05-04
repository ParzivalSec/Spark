#pragma once

// Memory Sys
#include "MemorySystem/RawAllocators_Benchmarks.h"
#include "MemorySystem/MemoryRealm_Linear_Unsafe.h"
// Containers
#include "Containers/Vector_Benchmarks.h"
#include "Containers/Handlemap_Benchmarks.h"

typedef void(*BenchmarkScenarioFunction)();

const BenchmarkScenarioFunction scenarios[] =
{
	// Allocator benchmarks
	&allocate_100_data_objects_new,						// ID 0
	&allocate_100_data_objects_linear,					// ID 1
	&allocate_100_data_objects_stack,					// ID 2
	&allocate_100_data_objects_double_ended_stack,		// ID 3
	&allocate_100_data_objects_pool,					// ID 4
	// Realm benchmark
	&memory_realm_linear_100_objects_unsafe,			// ID 5
	// Vector benchmarks
	&vec_1000_without_cap,								// ID 6
	&vec_1000_with_cap,									// ID 7
	&vec_1000_iteration,								// ID 8
	&vec_1000_erase_range,								// ID 9
	// HandleMap benchmarks
	&handlemap_1000_insertion,							// ID 10
	&handlemap_1000_iteration,							// ID 11
	&handlemap_1000_remove,								// ID 12
	// Ringbuffer benchmarks
};