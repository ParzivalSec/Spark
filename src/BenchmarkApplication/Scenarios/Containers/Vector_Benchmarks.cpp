#include "Vector_Benchmarks.h"
#include "Vector/Vector.h"
#include "../CommonStruct.h"

static const size_t VEC_COUNT = 1000;

void vec_1000_without_cap()
{
	sp::container::Vector<AllocationData> vec;

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		vec.push_back(AllocationData());
	}
}

void vec_1000_with_cap()
{
	sp::container::Vector<AllocationData> vec;

	vec.reserve(VEC_COUNT);

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		vec.push_back(AllocationData());
	}
}

void vec_1000_iteration()
{
	sp::container::Vector<AllocationData> vec;

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		vec.push_back(AllocationData());
	}

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		vec[idx].data_block_1[0] = 10;
	}
}

void vec_1000_erase_range()
{
	sp::container::Vector<AllocationData> vec;

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		vec.push_back(AllocationData());
	}

	vec.erase(0, 500);
}
