#include <new>

#include "RawAllocators_Benchmarks.h"
#include "../CommonStruct.h"
#include "Allocator/NonGrowing/LinearAllocator.h"
#include "Allocator/NonGrowing/StackAllocator.h"
#include "Allocator/NonGrowing/DoubleEndedStackAllocator.h"
#include "Allocator/NonGrowing/PoolAllocator.h"

static const size_t NUM_ALLOC_OBJ = 100;
static const size_t LINEAR_ALLOC_OVERHEAD = 4;
static const size_t STACK_ALLOC_OVERHEAD = 8;

void allocate_100_data_objects_new()
{
	AllocationData* allocations[NUM_ALLOC_OBJ];

	for (size_t idx = 0; idx < NUM_ALLOC_OBJ; ++idx)
	{
		allocations[idx] = new AllocationData;
	}

	for (AllocationData* allocation : allocations)
	{
		delete allocation;
	}
}

void allocate_100_data_objects_linear()
{
	AllocationData* allocations[NUM_ALLOC_OBJ];
	sp::memory::LinearAllocator linearAlloc(NUM_ALLOC_OBJ * (sizeof(AllocationData) + LINEAR_ALLOC_OVERHEAD));

	for (size_t idx = 0; idx < NUM_ALLOC_OBJ; ++idx)
	{
		void* raw_mem = linearAlloc.Alloc(sizeof(AllocationData), 1, 0);
	}

	for (AllocationData* allocation : allocations)
	{
		allocation->~AllocationData();
		linearAlloc.Dealloc(allocation);
	}
}

void allocate_100_data_objects_stack()
{
	AllocationData* allocations[NUM_ALLOC_OBJ];
	sp::memory::StackAllocator stackAlloc(NUM_ALLOC_OBJ * (sizeof(AllocationData) + STACK_ALLOC_OVERHEAD));

	for (size_t idx = 0; idx < NUM_ALLOC_OBJ; ++idx)
	{
		void* raw_mem = stackAlloc.Alloc(sizeof(AllocationData), 1, 0);
		allocations[idx] = new (raw_mem) AllocationData;
	}

	for (AllocationData* allocation : allocations)
	{
		allocation->~AllocationData();
		stackAlloc.Dealloc(allocation);
	}
}

void allocate_100_data_objects_double_ended_stack()
{
	AllocationData* allocations[NUM_ALLOC_OBJ];
	sp::memory::DoubleEndedStackAllocator deStackAlloc(NUM_ALLOC_OBJ * (sizeof(AllocationData) + STACK_ALLOC_OVERHEAD));

	for (size_t idx = 0; idx < NUM_ALLOC_OBJ; ++idx)
	{
		void* raw_mem = deStackAlloc.Alloc(sizeof(AllocationData), 1, 0);
		allocations[idx] = new (raw_mem) AllocationData;
	}

	for (AllocationData* allocation : allocations)
	{
		allocation->~AllocationData();
		deStackAlloc.Dealloc(allocation);
	}
}

void allocate_100_data_objects_pool()
{
	AllocationData* allocations[NUM_ALLOC_OBJ];
	sp::memory::PoolAllocator poolAlloc(sizeof(AllocationData), 100, 1, 0);

	for (size_t idx = 0; idx < NUM_ALLOC_OBJ; ++idx)
	{
		void* raw_mem = poolAlloc.Alloc(sizeof(AllocationData), 1, 0);
		allocations[idx] = new (raw_mem) AllocationData;
	}

	for (AllocationData* allocation : allocations)
	{
		allocation->~AllocationData();
		poolAlloc.Dealloc(allocation);
	}
}
