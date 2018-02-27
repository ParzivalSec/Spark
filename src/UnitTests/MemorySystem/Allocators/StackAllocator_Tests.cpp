#include "gtest/gtest.h"

#include "Allocator/NonGrowing/StackAllocator.h"
#include "PointerUtil.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;
const size_t ONE_GIBIBYTE = 1024 * ONE_MIBIBYTE;

TEST(StackAllocator_NonGrowing, SingleAllocation)
{
	sp::memory::StackAllocator stackAllocator(ONE_MIBIBYTE * 10);
	void* raw_mem = stackAllocator.Alloc(256, 4, 0);
	ASSERT_NE(raw_mem, nullptr) << "StackAllocator shall return a valid pointer";
}

TEST(StackAllocator_NonGrowing, SingleAllocation_Right_Alignment)
{
	sp::memory::StackAllocator stackAllocator(ONE_MIBIBYTE * 10);
	void* raw_mem = stackAllocator.Alloc(256, 16, 0);
	ASSERT_EQ(sp::pointerUtil::IsAlignedTo(raw_mem, 16), true) << "Raw_mem should be aligned to a 4-byte boundary";
}

TEST(StackAllocator_NonGrowing, MultipleAllocation)
{
	sp::memory::StackAllocator stackAllocator(ONE_MIBIBYTE * 10);
	void* raw_mem_1 = stackAllocator.Alloc(ONE_MIBIBYTE, 16, 0);
	ASSERT_NE(raw_mem_1, nullptr) << "StackAllocator shall return a valid pointer for raw_mem_1";
	void* raw_mem_2 = stackAllocator.Alloc(ONE_MIBIBYTE, 16, 0);
	ASSERT_NE(raw_mem_2, nullptr) << "StackAllocator shall return a valid pointer for raw_mem_2";
	void* raw_mem_3 = stackAllocator.Alloc(ONE_MIBIBYTE, 16, 0);
	ASSERT_NE(raw_mem_3, nullptr) << "StackAllocator shall return a valid pointer for raw_mem_3";
	void* raw_mem_4 = stackAllocator.Alloc(ONE_MIBIBYTE, 16, 0);
	ASSERT_NE(raw_mem_4, nullptr) << "StackAllocator shall return a valid pointer for raw_mem_4";
}

TEST(StackAllocator_NonGrowing, ReturnsNullptr_on_OOM)
{
	sp::memory::StackAllocator stackAllocator(ONE_MIBIBYTE * 10);
	void* raw_mem_1 = stackAllocator.Alloc(ONE_MIBIBYTE * 6, 4, 0);
	void* raw_mem_2 = stackAllocator.Alloc(ONE_MIBIBYTE * 6, 4, 0);
	ASSERT_EQ(raw_mem_2, nullptr) << "StackAllocator shall return nullptr on oom";
}

TEST(StackAllocator_NonGrowing, ReverseFree)
{
	sp::memory::StackAllocator stackAllocator(ONE_MIBIBYTE * 10);
	void* raw_mem_1 = stackAllocator.Alloc(ONE_MIBIBYTE * 4, 4, 0);
	void* raw_mem_2 = stackAllocator.Alloc(ONE_MIBIBYTE * 4, 4, 0);
	// TODO: Proper test LIFO freeing here
}

TEST(StackAllocator_NonGrowing, ResetAllocator)
{
	sp::memory::StackAllocator stackAllocator(ONE_MIBIBYTE * 10);
	void* raw_mem_1 = stackAllocator.Alloc(ONE_MIBIBYTE * 4, 4, 0);
	void* raw_mem_2 = stackAllocator.Alloc(ONE_MIBIBYTE * 4, 4, 0);
	uint32_t mem = 0xDEADBEEF;

	memcpy(raw_mem_1, &mem, sizeof(uint32_t));
	memcpy(raw_mem_2, &mem, sizeof(uint32_t));

	stackAllocator.Reset();

	void* raw_mem_3 = stackAllocator.Alloc(ONE_MIBIBYTE * 4, 4, 0);
	void* raw_mem_4 = stackAllocator.Alloc(ONE_MIBIBYTE * 4, 4, 0);

	uint32_t new_mem = 0xFFEEFFEE;
	memcpy(&new_mem, raw_mem_3, sizeof(uint32_t));
	ASSERT_EQ(mem, new_mem);

	new_mem = 0xFFEEFFEE;
	memcpy(&new_mem, raw_mem_4, sizeof(uint32_t));
	ASSERT_EQ(mem, new_mem);
}