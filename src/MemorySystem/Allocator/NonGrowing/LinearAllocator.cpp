#include "LinearAllocator.h"

#include <cassert>

#include "../../VirtualMemory/VirtualMemory.h"

sp::memory::LinearAllocator::LinearAllocator(size_t size)
{
	m_memoryBegin.as_void = ReserveAddressSpace(size);
	m_memoryBegin.as_void = CommitPhysicalMemory(m_memoryBegin.as_void, size);
}

sp::memory::LinearAllocator::LinearAllocator(void* memoryStart, void* memoryEnd)
{
	{
		bool isValidMemoryRange = memoryStart < memoryEnd;
		assert(isValidMemoryRange && "Memory end is not allowed to be lesser or equal than memory start");
	}

	m_memoryBegin.as_void = memoryStart;
	m_memoryEnd.as_void = memoryEnd;
}

void* sp::memory::LinearAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	return nullptr;
}

void sp::memory::LinearAllocator::Dealloc(void* memory)
{
	// TODO: Implement function
}

void sp::memory::LinearAllocator::Reset()
{
	// TODO: Implement function
}

sp::memory::LinearAllocator::~LinearAllocator()
{
	// ...
}
