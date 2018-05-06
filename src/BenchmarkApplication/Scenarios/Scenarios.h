#pragma once

// Memory Sys
#include "MemorySystem/RawAllocators_Benchmarks.h"
#include "MemorySystem/MemoryRealm_Linear_Unsafe.h"
// Containers
#include "Containers/Vector_Benchmarks.h"
#include "Containers/Handlemap_Benchmarks.h"
#include "Containers/Ringbuffer_Benchmarks.h"
// ECS
#include "ECS/ECS_Benchmarks.h"

typedef void(*BenchmarkScenarioFunction)();

const BenchmarkScenarioFunction scenarios[] =
{
	// Allocator benchmarks
	&allocate_1000_data_objects_new,					// ID 0
	&allocate_1000_data_objects_linear,					// ID 1
	&allocate_1000_data_objects_stack,					// ID 2
	&allocate_1000_data_objects_double_ended_stack,		// ID 3
	&allocate_1000_data_objects_pool,					// ID 4
	// Realm benchmark
	&memory_realm_linear_1000_objects_unsafe,			// ID 5
	// Vector benchmarks
	&vec_10000_without_cap,								// ID 6
	&vec_10000_with_cap,								// ID 7
	&vec_10000_iteration,								// ID 8
	&vec_10000_erase_range,								// ID 9
	// HandleMap benchmarks
	&handlemap_10000_insertion,							// ID 10
	&handlemap_10000_iteration,							// ID 11
	&handlemap_10000_remove,							// ID 12
	// Ringbuffer benchmarks
	&ringbuffer_1000_write,								// ID 13
	&ringbuffer_1000_read,								// ID 14
	&ringbuffer_500_write_wrapping,						// ID 15
	// ECS
	&ecs_create_10000_with_pos,							// ID 16
	&ecs_create_100000_with_pos,						// ID 17
	&ecs_create_10000_pos_vel,							// ID 18
	&ecs_create_100000_pos_vel,							// ID 19
	&ecs_iterate_10000_pos,								// ID 20
	&ecs_iterate_100000_pos,							// ID 21
	&ecs_remove_5000_pos,								// ID 22
	&ecs_remove_50000_pos,								// ID 23
};