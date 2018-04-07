#include "gtest/gtest.h"

#include "Vector/Vector.h"

TEST(Vector_Basic, Default_Construction)
{
	const sp::container::Vector<size_t> vec;
	ASSERT_EQ(vec.capacity(), 0u) << "Initial capacity was not 0";
	ASSERT_EQ(vec.size(), 0u) << "Initial capacity was not 0";
}

TEST(Vector_Basic, Push_Back)
{
	sp::container::Vector<size_t> vec;

	for (size_t idx = 0u; idx < 10; ++idx)
	{
		vec.push_back(idx);
	}

	for (size_t idx = 0u; idx < 10; ++idx)
	{
		ASSERT_EQ(vec[idx], idx) << "Vector value mismatch";
	}
}

// Be careful - in Debug this test needs ~ 15 Seconds
/*
TEST(Vector_Basic, Assert_On_Element_Limit_Exceeded)
{
	sp::container::Vector<size_t> vec;

	for (size_t idx = 0u; idx < 134217728; ++idx)
	{
		vec.push_back(idx);
	}

	ASSERT_DEATH(vec.push_back(666u), "Grow would exceed maximum available address space - cannot grow further!");
}
*/

TEST(Basic_Vector, Reserve)
{
	sp::container::Vector<uint32_t> vec;
	// We want to reserve spce for 100 32-bit ints (100 * 4 = 400 bytes mem), because we use VirtualMem
	// we can only allocate in page size chunks and will reserve here at least 4KB (4096 bytes) mem, so the
	// capacity shall be 1024 after the reserve call
	vec.reserve(100);
	ASSERT_EQ(vec.capacity(), 1024) << "Capacity did not match the expected grow behaviour";
}

TEST(Basic_Vector, Resize_Default_CTOR)
{
	sp::container::Vector<uint32_t> vec;
	vec.resize(100);
	ASSERT_EQ(vec.size(), 100) << "Size did not match the request resize size";
	ASSERT_EQ(vec.capacity(), 1024) << "Capacity did not match the expected grow behaviour";
}

TEST(Basic_Vector, Resize_With_CCTOR)
{
	sp::container::Vector<uint32_t> vec;
	vec.resize(100, 42u);

	ASSERT_EQ(vec.size(), 100) << "Size did not match the request resize size";
	ASSERT_EQ(vec.capacity(), 1024) << "Capacity did not match the expected grow behaviour";

	for (size_t idx = 0u; idx < 100; ++idx)
	{
		ASSERT_EQ(vec[idx], 42u) << "Resize did not initialize element with provided value";
	}
}

TEST(Vector_Basic, Copy_Construction)
{
	sp::container::Vector<size_t> vec_first;

	vec_first.push_back(123u);
	vec_first.push_back(456u);
	vec_first.push_back(789u);
	vec_first.push_back(666666u);

	sp::container::Vector<size_t> vec_copy(vec_first);
	ASSERT_EQ(vec_first.size(), vec_copy.size()) << "Size mismatch after copy construction";
	ASSERT_EQ(vec_first.capacity(), vec_copy.capacity()) << "Capacity mismatch after copy construction";

	ASSERT_EQ(vec_copy[0], 123u) << "Value at index 0 was corrupted";
	ASSERT_EQ(vec_copy[1], 456u) << "Value at index 1 was corrupted";
	ASSERT_EQ(vec_copy[2], 789u) << "Value at index 2 was corrupted";
	ASSERT_EQ(vec_copy[3], 666666u) << "Value at index 3 was corrupted";
}

TEST(Vector_Basic, Assign_Larger_To_Smaller_Vec)
{
	sp::container::Vector<size_t> vec_first;

	vec_first.push_back(123u);
	vec_first.push_back(456u);
	vec_first.push_back(789u);
	vec_first.push_back(666666u);

	sp::container::Vector<size_t> vec_copy;

	vec_copy.push_back(6161u);

	vec_copy = vec_first;
	
	ASSERT_EQ(vec_first.size(), vec_copy.size()) << "Size mismatch after assignment";
	ASSERT_EQ(vec_first.capacity(), vec_copy.capacity()) << "Capacity mismatch after assignment";

	ASSERT_EQ(vec_copy[0], 123u) << "Value at index 0 was corrupted";
	ASSERT_EQ(vec_copy[1], 456u) << "Value at index 1 was corrupted";
	ASSERT_EQ(vec_copy[2], 789u) << "Value at index 2 was corrupted";
	ASSERT_EQ(vec_copy[3], 666666u) << "Value at index 3 was corrupted";
}

TEST(Vector_Basic, Assign_Smaller_To_Larger_Vec)
{
	sp::container::Vector<size_t> vec_first;

	vec_first.push_back(1u);

	sp::container::Vector<size_t> vec_copy;

	vec_copy.push_back(123u);
	vec_copy.push_back(456u);
	vec_copy.push_back(789u);
	vec_copy.push_back(666666u);

	vec_copy = vec_first;

	ASSERT_EQ(vec_first.size(), vec_copy.size()) << "Size mismatch after assignment";
	ASSERT_EQ(vec_first.capacity(), vec_copy.capacity()) << "Capacity mismatch after assignment";

	ASSERT_EQ(vec_copy[0], 1u) << "Value at index 0 was corrupted";
}

TEST(Vector_Basic, Erase_Single_Element)
{
	sp::container::Vector<size_t> vec;

	vec.push_back(1u);
	vec.push_back(2u);
	vec.push_back(3u);
	vec.push_back(4u);

	vec.erase(1);

	ASSERT_EQ(vec.size(), 3u) << "Vector size did not change after erasing an element";
	ASSERT_EQ(vec[0], 1u) << "Index 0 was corrupted";
	ASSERT_EQ(vec[1], 3u) << "Index 1 was corrupted";
	ASSERT_EQ(vec[2], 4u) << "Index 2 was corrupted";
}

TEST(Vector_Basic, Erase_Elements_In_Range)
{
	sp::container::Vector<size_t> vec;

	vec.push_back(1u);
	vec.push_back(2u);
	vec.push_back(3u);
	vec.push_back(4u);

	vec.erase(1, 2);

	ASSERT_EQ(vec.size(), 2u) << "Vector size did not change after erasing an element";
	ASSERT_EQ(vec[0], 1u) << "Index 0 was corrupted";
	ASSERT_EQ(vec[1], 4u) << "Index 1 was corrupted";
}

TEST(Vector_Basic, Erase_Empty_Range)
{
	sp::container::Vector<size_t> vec;

	vec.push_back(1u);
	vec.push_back(2u);
	vec.push_back(3u);
	vec.push_back(4u);

	vec.erase(1, 1);

	ASSERT_EQ(vec.size(), 4u) << "Vector size did not change after erasing an element";
	ASSERT_EQ(vec[0], 1u) << "Index 0 was corrupted";
	ASSERT_EQ(vec[1], 2u) << "Index 1 was corrupted";
	ASSERT_EQ(vec[2], 3u) << "Index 2 was corrupted";
	ASSERT_EQ(vec[3], 4u) << "Index 3 was corrupted";
}

TEST(Vector_Basic, Erase_By_Swap)
{
	sp::container::Vector<size_t> vec;

	vec.push_back(1u);
	vec.push_back(2u);
	vec.push_back(3u);
	vec.push_back(4u);

	vec.erase_by_swap(1);

	ASSERT_EQ(vec.size(), 3u) << "Vector size did not change after erasing an element";
	ASSERT_EQ(vec[0], 1u) << "Index 0 was corrupted";
	ASSERT_EQ(vec[1], 4u) << "Index 1 was corrupted";
	ASSERT_EQ(vec[2], 3u) << "Index 2 was corrupted";
}