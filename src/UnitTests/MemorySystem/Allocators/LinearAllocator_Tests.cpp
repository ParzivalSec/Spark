#include "gtest/gtest.h"

#include "Allocator/NonGrowing/LinearAllocator.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;
const size_t ONE_GIBIBYTE = 1024 * ONE_MIBIBYTE;

TEST(LinearAllocator_NonGrowing, SingleAllocation)
{
	sp::memory::LinearAllocator linearAlloc(ONE_MIBIBYTE * 10);

	void* memRaw = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 4, 0);

	ASSERT_NE(memRaw, nullptr) << "LinearAllocator could not allocate one mibibyte from 10 MB";
}

TEST(LinearAllocator_NonGrowing, MultipleAllocations)
{
	sp::memory::LinearAllocator linearAlloc(ONE_MIBIBYTE * 10);

	void* memRaw = nullptr;

	memRaw = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 4, 0);
	ASSERT_NE(memRaw, nullptr) << "LinearAllocator failed to allocate first request";
	memRaw = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 4, 0);
	ASSERT_NE(memRaw, nullptr) << "LinearAllocator failed to allocate second request";
	memRaw = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 4, 0);
	ASSERT_NE(memRaw, nullptr) << "LinearAllocator failed to allocate third request";
}

TEST(LinearAllocator_NonGrowing, ResetWholeAllocator)
{
	sp::memory::LinearAllocator linearAlloc(ONE_MIBIBYTE * 10);

	void* memRaw_beforeReset = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 4, 0);
	linearAlloc.Reset();
	void* memRaw_afterReset  = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 4, 0);
	ASSERT_EQ(memRaw_beforeReset, memRaw_afterReset) << "LinearAllocator was not properly reset";
}