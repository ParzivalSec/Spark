#include <new>

#include "MemoryRealm_Linear_Unsafe.h"
#include "../CommonStruct.h"

#include "MemoryRealm/MemoryRealm.h"
#include "Allocator/NonGrowing/LinearAllocator.h"
#include "BoundsChecker/SimpleBoundsChecker.h"

static const size_t NUM_ALLOC_OBJ = 100;
static const size_t ALLOCATOR_OVERHEAD = 4;
static const size_t CANARY_OVERHEAD = 8;

typedef sp::memory::MemoryRealm<sp::memory::LinearAllocator, sp::memory::SimpleBoundsChecker> LinearBoundsCheckingRealm;

void memory_realm_linear_100_objects_unsafe()
{
	AllocationData* allocations[NUM_ALLOC_OBJ];
	LinearBoundsCheckingRealm realm(100 * (sizeof(AllocationData) + ALLOCATOR_OVERHEAD + CANARY_OVERHEAD));

	for (size_t idx = 0; idx < NUM_ALLOC_OBJ; ++idx)
	{
		void* raw_mem = realm.Alloc(sizeof(AllocationData), 1);
		allocations[idx] = new (raw_mem) AllocationData;
	}

	for (AllocationData* allocation : allocations)
	{
		allocation->~AllocationData();
		realm.Dealloc(allocation);
	}
}