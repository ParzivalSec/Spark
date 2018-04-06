#include "gtest/gtest.h"

#include "Allocator/NonGrowing/PoolAllocator.h"
#include "PointerUtil.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;

struct TestObject
{
	uint32_t foo[1000];
	uint32_t bar[1000];
};

/// Tests without alignment and offset
TEST(PoolAllocator_NonGrowing, Allocate_Single_Object_Unaligned)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject), 10, 1, 0);
	void* raw_mem = pool.Alloc(sizeof(TestObject), 1, 0);
	ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
}

TEST(PoolAllocator_NonGrowing, Allocate_Multiple_Objects_Unaligned)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject), 10, 1, 0);

	for (size_t i = 0; i < 10; ++i)
	{
		void* raw_mem = pool.Alloc(sizeof(TestObject), 1, 0);
		ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
	}
}

/// Tests with alignment but without offset
TEST(PoolAllocator_NonGrowing, Allocate_Single_Object_Aligned_4)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject), 10, 4, 0);
	void* raw_mem = pool.Alloc(sizeof(TestObject), 4, 0);
	ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 4)) << "Pointer was not aligned to a 4";
}

TEST(PoolAllocator_NonGrowing, Allocate_Multiple_Objects_Aligned_4)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject), 10, 4, 0);

	for (size_t i = 0; i < 10; ++i)
	{
		void* raw_mem = pool.Alloc(sizeof(TestObject), 4, 0);
		ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
		ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 4)) << "Pointer was not aligned to a 4";
	}
}

TEST(PoolAllocator_NonGrowing, Allocate_Single_Object_Aligned_32)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject), 10, 32, 0);
	void* raw_mem = pool.Alloc(sizeof(TestObject), 32, 0);
	ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 32)) << "Pointer was not aligned to a 32";
}

TEST(PoolAllocator_NonGrowing, Allocate_Multiple_Objects_Aligned_32)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject), 10, 32, 0);

	for (size_t i = 0; i < 10; ++i)
	{
		void* raw_mem = pool.Alloc(sizeof(TestObject), 32, 0);
		ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
		ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 32)) << "Pointer was not aligned to a 32";
	}
}

/// Tests with alignment & with offset
TEST(PoolAllocator_NonGrowing, Allocate_Single_Object_Aligned_With_Offset)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject) + 8, 10, 32, 4);
	void* raw_mem = pool.Alloc(sizeof(TestObject) + 8, 32, 4);
	ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
	ASSERT_FALSE(sp::pointerUtil::IsAlignedTo(raw_mem, 32)) << "Offsetted pointer is already aligned to 32";
	char* userPointerWithoutOffset = sp::pointerUtil::pseudo_cast<char*>(raw_mem, 4);
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(userPointerWithoutOffset, 32)) << "User pointer was not aligned to a 32";
}

TEST(PoolAllocator_NonGrowing, Allocate_Multiple_Objects_Aligned_With_Offset)
{
	sp::memory::PoolAllocator pool(sizeof(TestObject) + 8, 10, 32, 4);

	for (size_t i = 0; i < 10; ++i)
	{
		void* raw_mem = pool.Alloc(sizeof(TestObject) + 8, 32, 4);
		ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
		ASSERT_FALSE(sp::pointerUtil::IsAlignedTo(raw_mem, 32)) << "Offsetted pointer is already aligned to 32";
		char* userPointerWithoutOffset = sp::pointerUtil::pseudo_cast<char*>(raw_mem, 4);
		ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(userPointerWithoutOffset, 32)) << "User pointer was not aligned to a 32";
	}
}

/// Tests with external memory / Aligned and with offset on stack
TEST(PoolAllocator_NonGrowing, Allocate_Multiple_Objects_Aligned_From_External_Mem)
{
	// Declare some scratch memory on the stack that shall be aligned to 32
	// Has place for 16 objects of type TestObject
	constexpr size_t memSizeForObjects = 16 * (sizeof(TestObject) + 32) + 7996;
	__declspec(align(32)) char stackMemory[memSizeForObjects];
	ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(stackMemory, 32)) << "Stack memory was not aligned properly";

	sp::memory::PoolAllocator pool(stackMemory, stackMemory + memSizeForObjects, sizeof(TestObject), 32, 0);

	for (size_t i = 0; i < 16; ++i)
	{
		void* raw_mem = pool.Alloc(sizeof(TestObject), 32, 0);
		ASSERT_NE(raw_mem, nullptr) << "PoolAllocator did not return a valid pointer";
		ASSERT_TRUE(sp::pointerUtil::IsAlignedTo(raw_mem, 32)) << "User pointer was not aligned to a 32";
	}
}

TEST(PoolAllocator_NonGrowing, Allocations_Do_Not_Invalidate_Objects)
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
	sp::memory::PoolAllocator poolAllocator(sizeof(Data), 10, 1, 0);

	for (size_t idx = 0; idx < 10; ++idx)
	{
		data[idx] = new (poolAllocator.Alloc(sizeof(Data), 1, 0)) Data(idx);
	}

	for (size_t idx = 0; idx < 10; ++idx)
	{
		ASSERT_TRUE(data[idx]->some == idx);
		ASSERT_TRUE(data[idx]->value == idx);
		ASSERT_TRUE(data[idx]->without == idx);
		ASSERT_TRUE(data[idx]->meaning == idx);
	}
}