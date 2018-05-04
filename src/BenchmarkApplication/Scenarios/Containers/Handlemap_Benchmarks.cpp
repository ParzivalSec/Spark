#include "Handlemap_Benchmarks.h"

#include <vector>

#include "HandleMap/HandleMap.h"
#include "../CommonStruct.h"

static const size_t VEC_COUNT = 1000;

void handlemap_1000_insertion()
{
	sp::container::HandleMap<AllocationData> handlemap(1000);

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		handlemap.Insert(AllocationData());
	}
}

void handlemap_1000_iteration()
{
	sp::container::HandleMap<AllocationData> handlemap(1000);
	std::vector<sp::container::Handle> handles(1000);

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		handles.push_back(handlemap.Insert(AllocationData()));
	}

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		handlemap[handles[idx]].data_block_1[0] = 10;
	}
}

void handlemap_1000_remove()
{
	sp::container::HandleMap<AllocationData> handlemap(1000);
	std::vector<sp::container::Handle> handles(1000);

	for (size_t idx = 0; idx < VEC_COUNT; ++idx)
	{
		handles.push_back(handlemap.Insert(AllocationData()));
	}

	for (size_t idx = 0; idx < VEC_COUNT / 2; ++idx)
	{
		handlemap.Erase(handles[idx]);
	}
}
