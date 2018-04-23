#pragma once

#include "LinearAllocator_Benchmarks.h"
#include "Allocator/NonGrowing/LinearAllocator.h"
#include <cstring>

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

void allocate_50MB_unaligned_ten_times()
{
	sp::memory::LinearAllocator linearAlloc(400 + ONE_MIBIBYTE * 50 * 100);

	for (size_t i = 0; i < 100; ++i)
	{
		void* raw = linearAlloc.Alloc(ONE_MIBIBYTE * 50, 1, 0);
		memset(raw, 0, ONE_MIBIBYTE * 50);
	}
}

void allocate_space_for_50_small_objects()
{
}

void allocate_space_for_50_medium_objects()
{
}

void allocate_space_for_50_large_objects()
{
}
