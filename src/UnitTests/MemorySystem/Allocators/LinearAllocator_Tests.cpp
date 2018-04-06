#include "gtest/gtest.h"

#include "Allocator/NonGrowing/LinearAllocator.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;

TEST(LinearAllocator_NonGrowing, Single_Allocation)
{
	sp::memory::LinearAllocator linearAlloc(ONE_MIBIBYTE * 10);

	void* memRaw = linearAlloc.Alloc(ONE_MIBIBYTE * 1, 1, 0);

	ASSERT_NE(memRaw, nullptr) << "LinearAllocator could not allocate one mibibyte from 10 MB";
}

TEST(LinearAllocator_NonGrowing, Multiple_Allocations)
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

TEST(LinearAllocator_NonGrowing, Return_Right_Allocation_Size)
{
	sp::memory::LinearAllocator linearAlloc(ONE_MIBIBYTE * 10);

	void* raw_mem_0 = linearAlloc.Alloc(ONE_MIBIBYTE * 2, 1, 0);
	ASSERT_TRUE(linearAlloc.GetAllocationSize(raw_mem_0) == ONE_MIBIBYTE * 2);
	void* raw_mem_1 = linearAlloc.Alloc(ONE_MIBIBYTE * 3, 1, 0);
	ASSERT_TRUE(linearAlloc.GetAllocationSize(raw_mem_1) == ONE_MIBIBYTE * 3);
	void* raw_mem_2 = linearAlloc.Alloc(ONE_MIBIBYTE * 4, 1, 0);
	ASSERT_TRUE(linearAlloc.GetAllocationSize(raw_mem_2) == ONE_MIBIBYTE * 4);
}

TEST(LinearAllocator_NonGrowing, Allocations_Do_Not_Invalidate_Objects)
{
	struct Data
	{
		Data(size_t val) 
			: some(val)
			, value(val)
			, without(val)
			, meaning(val)
		{}

		size_t some;
		size_t value;
		size_t without;
		size_t meaning;
	};

	Data* data[10];
	sp::memory::LinearAllocator linearAllocator(ONE_MIBIBYTE);
	
	for (size_t idx = 0; idx < 10; ++idx)
	{
		data[idx] = new (linearAllocator.Alloc(sizeof(Data), 1, 0)) Data(idx);
	}

	for (size_t idx = 0; idx < 10; ++idx)
	{
		ASSERT_TRUE(data[idx]->some == idx);
		ASSERT_TRUE(data[idx]->value == idx);
		ASSERT_TRUE(data[idx]->without == idx);
		ASSERT_TRUE(data[idx]->meaning == idx);
	}
}