#include "gtest/gtest.h"

#include "Allocator/NonGrowing/LinearAllocator.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;
const size_t ONE_GIBIBYTE = 1024 * ONE_MIBIBYTE;

TEST(LinearAllocator_NonGrowing, SingleWholeBlockAllocation)
{
	sp::memory::LinearAllocator linearAlloc(ONE_MIBIBYTE * 10);

	void* memRaw = linearAlloc.Alloc(ONE_MIBIBYTE * 10, 0, 0);

	ASSERT_NE(memRaw, nullptr) << "LinearAllocator could not return the whole 10MB memory block";
}

TEST(LinearAllocator_NonGrowing, SingleAllocationUnaligned) {}

TEST(LinearAllocator_NonGrowing, SingleAllocationAligned) {}

TEST(LinearAllocator_NonGrowing, MultipleAllocationUnaligned) {}

TEST(LinearAllocator_NonGrowing, MultipleAllocationAligned) {}

TEST(LinearAllocator_NonGrowing, EmptyFree) {}

TEST(LinearAllocator_NonGrowing, ResetWholeAllocator) {}