#include <cstring>
#include "LinearAllocator_Benchmarks.h"
#include "Allocator/NonGrowing/LinearAllocator.h"
#include <cstdint>
#include <new>

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;

void allocate_100KB_unaligned_hundred_times()
{
	sp::memory::LinearAllocator linearAlloc(400 + ONE_KIBIBYTE * 100 * 100);

	for (size_t i = 0; i < 100; ++i)
	{
		void* raw = linearAlloc.Alloc(ONE_KIBIBYTE * 100, 1, 0);
		memset(raw, 0, ONE_KIBIBYTE * 100);
	}
}

void allocate_1MB_unaligned_hundred_times()
{
	sp::memory::LinearAllocator linearAlloc(400 + ONE_MIBIBYTE * 100);

	for (size_t i = 0; i < 100; ++i)
	{
		void* raw = linearAlloc.Alloc(ONE_MIBIBYTE, 1, 0);
		memset(raw, 0, ONE_MIBIBYTE);
	}
}

void allocate_100MB_unaligned_ten_times()
{
	sp::memory::LinearAllocator linearAlloc(400 + ONE_MIBIBYTE * 100 * 10);

	for (size_t i = 0; i < 10; ++i)
	{
		void* raw = linearAlloc.Alloc(ONE_MIBIBYTE * 100, 1, 0);
		memset(raw, 0, ONE_MIBIBYTE * 100);
	}
}

// Object allocation benchmarks

struct DataPackage_Large
{
	size_t uniqueId;
	uint8_t buffer[2 * ONE_KIBIBYTE];
};

void allocate_space_for_50_large_objects_new()
{
	DataPackage_Large* objects[50];

	sp::memory::LinearAllocator linearAlloc(200 + 50 * sizeof(DataPackage_Large));

	for (DataPackage_Large* object : objects)
	{
		object = new DataPackage_Large;
	}

	for (DataPackage_Large* object : objects)
	{
		delete object;
	}
}

void allocate_space_for_50_large_objects()
{
	DataPackage_Large* objects[50];

	sp::memory::LinearAllocator linearAlloc(200 + 50 * sizeof(DataPackage_Large));

	for (DataPackage_Large* object : objects)
	{
		void* raw = linearAlloc.Alloc(sizeof(DataPackage_Large), 1, 0);
		object = new (raw) DataPackage_Large;
	}

	for (DataPackage_Large* object : objects)
	{
		object->~DataPackage_Large();
		linearAlloc.Dealloc(object);
	}
}
