#include "Handlemap_Benchmarks.h"

#include <vector>

#include "HandleMap/HandleMap.h"
#include "../CommonStruct.h"

static const size_t MAP_COUNT = 10000;

void handlemap_10000_insertion()
{
	sp::container::HandleMap<AllocationData> handlemap(MAP_COUNT);

	for (size_t idx = 0; idx < MAP_COUNT; ++idx)
	{
		handlemap.Insert(AllocationData());
	}
}

void handlemap_10000_iteration()
{
	sp::container::HandleMap<AllocationData> handlemap(MAP_COUNT);
	std::vector<sp::container::Handle> handles(MAP_COUNT);

	for (size_t idx = 0; idx < MAP_COUNT; ++idx)
	{
		handles.push_back(handlemap.Insert(AllocationData()));
	}

	for (size_t idx = 0; idx < MAP_COUNT; ++idx)
	{
		handlemap[handles[idx]].data_block_1[0] = 10;
	}
}

void handlemap_10000_remove()
{
	sp::container::HandleMap<AllocationData> handlemap(MAP_COUNT);
	std::vector<sp::container::Handle> handles(MAP_COUNT);

	for (size_t idx = 0; idx < MAP_COUNT; ++idx)
	{
		handles.push_back(handlemap.Insert(AllocationData()));
	}

	for (size_t idx = 0; idx < MAP_COUNT / 2; ++idx)
	{
		handlemap.Erase(handles[idx]);
	}
}
