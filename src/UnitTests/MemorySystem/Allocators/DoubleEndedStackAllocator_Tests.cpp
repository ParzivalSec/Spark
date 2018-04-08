#include "gtest/gtest.h"

#include "Allocator/NonGrowing/DoubleEndedStackAllocator.h"
#include "Pointers/PointerUtil.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;

TEST(DoubleEndedStack_NonGrowing, Allocate_Front_Single)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem = DEStack.Alloc(ONE_MIBIBYTE / 2, 1, 0);
	ASSERT_NE(raw_mem, nullptr) << "Allocating from front end should return a valid pointer.";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Front_Multiple)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem_1 = DEStack.Alloc(ONE_MIBIBYTE / 4, 1, 0);
	ASSERT_NE(raw_mem_1, nullptr) << "First allocation from front end should return a valid pointer.";
	void* raw_mem_2 = DEStack.Alloc(ONE_MIBIBYTE / 4, 1, 0);
	ASSERT_NE(raw_mem_2, nullptr) << "Second allocation from front end should return a valid pointer.";
	void* raw_mem_3 = DEStack.Alloc(ONE_MIBIBYTE / 4, 1, 0);
	ASSERT_NE(raw_mem_3, nullptr) << "Third allocation from front end should return a valid pointer.";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Front_Aligned)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem = DEStack.Alloc(ONE_MIBIBYTE / 2, 16, 0);
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 16)) << "Returned address should be a multiple of 16";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Front_With_Offset_Aligned)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem = DEStack.Alloc(ONE_MIBIBYTE / 2, 16, 8);
	char* raw_mem_as_char = sp::pointerUtil::pseudo_cast<char*>(raw_mem, 0);
	raw_mem_as_char += 8;
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem_as_char, 16)) << "Returned address should be a multiple of 16 although an offset was provided";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Back_Single)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem = DEStack.AllocBack(ONE_MIBIBYTE / 2, 1, 0);
	ASSERT_NE(raw_mem, nullptr) << "Allocating from back end should return a valid pointer.";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Back_Multiple)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem_1 = DEStack.AllocBack(ONE_MIBIBYTE / 4, 1, 0);
	ASSERT_NE(raw_mem_1, nullptr) << "First allocation from back end should return a valid pointer.";
	void* raw_mem_2 = DEStack.AllocBack(ONE_MIBIBYTE / 4, 1, 0);
	ASSERT_NE(raw_mem_2, nullptr) << "Second allocation from back end should return a valid pointer.";
	void* raw_mem_3 = DEStack.AllocBack(ONE_MIBIBYTE / 4, 1, 0);
	ASSERT_NE(raw_mem_3, nullptr) << "Third allocation from back end should return a valid pointer.";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Back_Aligned)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem = DEStack.AllocBack(ONE_MIBIBYTE / 2, 16, 0);
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 16)) << "Returned address should be a multiple of 16";
}

TEST(DoubleEndedStack_NonGrowing, Allocate_Back_With_Offset_Aligned)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem = DEStack.AllocBack(ONE_MIBIBYTE / 2, 16, 8);
	char* raw_mem_as_char = sp::pointerUtil::pseudo_cast<char*>(raw_mem, 0);
	raw_mem_as_char += 8;
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem_as_char, 16)) << "Returned address should be a multiple of 16 although an offset was provided";
}

TEST(DoubleEndedStack_NonGrowing, Check_Overlap_On_Alloc)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	void* raw_mem_1 = DEStack.Alloc(ONE_MIBIBYTE / 2, 1, 0);
	void* raw_mem_2 = DEStack.AllocBack(ONE_MIBIBYTE / 2, 1, 4);

	ASSERT_EQ(raw_mem_2, nullptr) << "Overlap check should have returned a nullptr on the second allocation";
}

TEST(DoubleEndedStack_NonGrowing, Deallocate_Front)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	const uint32_t memory_mark = 0xDEADBEEF;

	void* first_alloc = DEStack.Alloc(4, 1, 0);
	memcpy(first_alloc, &memory_mark, 4);
	DEStack.Dealloc(first_alloc);
	void* second_alloc = DEStack.Alloc(4, 1, 0);
	
	const uint32_t memory_mark_second = *sp::pointerUtil::pseudo_cast<uint32_t*>(second_alloc, 0);
	ASSERT_EQ(memory_mark, memory_mark_second) << "Allocator did not return the proper address after free from front";
}

TEST(DoubleEndedStack_NonGrowing, Deallocate_Back)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	const uint32_t memory_mark = 0xDEADBEEF;

	void* first_alloc = DEStack.AllocBack(4, 1, 0);
	memcpy(first_alloc, &memory_mark, 4);
	DEStack.DeallocBack(first_alloc);
	void* second_alloc = DEStack.AllocBack(4, 1, 0);

	const uint32_t memory_mark_second = *sp::pointerUtil::pseudo_cast<uint32_t*>(second_alloc, 0);
	ASSERT_EQ(memory_mark, memory_mark_second) << "Allocator did not return the proper address after free from back";
}

TEST(DoubleEndedStack_NonGrowing, Deallocate_Front_Out_Of_Allocator_Range)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);
	void* stack_alloc = DEStack.Alloc(256, 1, 0);

	char* some_other_mem = new char[100];
	ASSERT_DEATH(DEStack.Dealloc(some_other_mem), "Not in memory range.");

	delete[] some_other_mem;
}

TEST(DoubleEndedStack_NonGrowing, Deallocate_Back_Out_Of_Allocator_Range)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);
	void* stack_alloc = DEStack.AllocBack(256, 1, 0);

	char* some_other_mem = new char[100];
	ASSERT_DEATH(DEStack.DeallocBack(some_other_mem), "Not in memory range.");

	delete[] some_other_mem;
}

TEST(DoubleEndedStack_NonGrowing, Deallocate_Back_From_Front)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);
	void* first_alloc = DEStack.AllocBack(4, 1, 0);
	ASSERT_DEATH(DEStack.Dealloc(first_alloc), "userPointerInFrontRange");
}

TEST(DoubleEndedStack_NonGrowing, Deallocate_Front_From_Back)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);
	void* first_alloc = DEStack.Alloc(4, 1, 0);
	ASSERT_DEATH(DEStack.DeallocBack(first_alloc), "userPointerInBackRange");
}

TEST(DoubleEndedStack_NonGrowing, Reset)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE);

	const uint32_t memory_mark = 0xDEADBEEF;

	void* alloc_front_first = DEStack.Alloc(4, 1, 0);
	void* alloc_back_first = DEStack.AllocBack(4, 1, 0);

	memcpy(alloc_front_first, &memory_mark, 4);
	memcpy(alloc_back_first, &memory_mark, 4);

	DEStack.Reset();

	void* alloc_front_second = DEStack.Alloc(4, 1, 0);
	void* alloc_back_second = DEStack.AllocBack(4, 1, 0);

	const uint32_t mem_mark_front = *sp::pointerUtil::pseudo_cast<uint32_t*>(alloc_front_second, 0);
	ASSERT_EQ(mem_mark_front, memory_mark) << "Front end did not allocate the proper pointer after Reset() was called once.";
	const uint32_t mem_mark_back = *sp::pointerUtil::pseudo_cast<uint32_t*>(alloc_back_second, 0);
	ASSERT_EQ(mem_mark_back, memory_mark) << "Back end did not allocate the proper pointer after Reset() was called once.";
}

TEST(DoubleEndedStack_NonGrowing, Return_Right_Allocation_Size_Front)
{
	sp::memory::DoubleEndedStackAllocator DEStack(ONE_MIBIBYTE * 10);
	void* raw_mem_0 = DEStack.Alloc(ONE_MIBIBYTE * 2, 1, 0);
	ASSERT_TRUE(DEStack.GetAllocationSize(raw_mem_0) == ONE_MIBIBYTE * 2) << "Did not return size of 2 MB";
	void* raw_mem_1 = DEStack.Alloc(ONE_MIBIBYTE * 3, 1, 0);
	ASSERT_TRUE(DEStack.GetAllocationSize(raw_mem_1) == ONE_MIBIBYTE * 3) << "Did not return size of 3 MB";
	void* raw_mem_2 = DEStack.Alloc(ONE_MIBIBYTE * 4, 1, 0);
	ASSERT_TRUE(DEStack.GetAllocationSize(raw_mem_2) == ONE_MIBIBYTE * 4) << "Did not return size of 4 MB";
}

TEST(DoubleEndedStack_NonGrowing, Front_Allocations_Do_Not_Invalidate_Objects)
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
	sp::memory::DoubleEndedStackAllocator de_stackAllocator(ONE_MIBIBYTE);

	for (size_t idx = 0; idx < 10; ++idx)
	{
		data[idx] = new (de_stackAllocator.Alloc(sizeof(Data), 1, 0)) Data(idx);
	}

	for (size_t idx = 0; idx < 10; ++idx)
	{
		ASSERT_TRUE(data[idx]->some == idx);
		ASSERT_TRUE(data[idx]->value == idx);
		ASSERT_TRUE(data[idx]->without == idx);
		ASSERT_TRUE(data[idx]->meaning == idx);
	}
}

TEST(DoubleEndedStack_NonGrowing, Back_Allocations_Do_Not_Invalidate_Objects)
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
	sp::memory::DoubleEndedStackAllocator de_stackAllocator(ONE_MIBIBYTE);

	for (size_t idx = 0; idx < 10; ++idx)
	{
		data[idx] = new (de_stackAllocator.AllocBack(sizeof(Data), 1, 0)) Data(idx);
	}

	for (size_t idx = 0; idx < 10; ++idx)
	{
		ASSERT_TRUE(data[idx]->some == idx);
		ASSERT_TRUE(data[idx]->value == idx);
		ASSERT_TRUE(data[idx]->without == idx);
		ASSERT_TRUE(data[idx]->meaning == idx);
	}
}