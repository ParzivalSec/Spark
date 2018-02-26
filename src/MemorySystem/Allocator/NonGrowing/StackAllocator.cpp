#include "StackAllocator.h"

#include <cassert>

#include "../../VirtualMemory/VirtualMemory.h"

namespace
{
	const uint32_t ALLOCATION_META_SIZE = sizeof(uint32_t);
}

sp::memory::StackAllocator::StackAllocator(size_t size)
	: m_usingInternalMemory(true)
{
	assert(size != 0 && "Cannot intialize an allocator with size 0");

	m_memoryBegin.as_void = ReserveAddressSpace(size);
	m_memoryBegin.as_void = CommitPhysicalMemory(m_memoryBegin.as_void, size);
	m_memoryEnd.as_ptr = m_memoryBegin.as_ptr + size;
	m_currentPtr = m_memoryBegin;
}

sp::memory::StackAllocator::StackAllocator(void* memoryStart, void* memoryEnd)
	: m_usingInternalMemory(false)
	, m_memoryBegin { memoryStart }
	, m_memoryEnd { memoryEnd }
	, m_currentPtr(m_memoryBegin)
{
	bool isValidMemoryRange = memoryStart < memoryEnd;
	assert(isValidMemoryRange && "Memory end is not allowed to be lesser or equal than memory start");
}

void* sp::memory::StackAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	assert(pointerUtil::IsPowerOfTwo(alignment) && "Alignment has to be a power-of-two");

	// 1. Align current + user offset + allocation meta size
	m_currentPtr.as_ptr + offset + ALLOCATION_META_SIZE;
	m_currentPtr.as_void = pointerUtil::AlignTop(m_currentPtr.as_void, alignment);
	// 2. Advance current pointer
	// 3. Write total allocation size (offset + alloca meta + alignment + size) into alloc meta
}

void sp::memory::StackAllocator::Dealloc(void* memory)
{
	pointerUtil::PointerType userPtr{ memory };

	{
		bool userPointerInAllocatorRange = userPtr.as_ptr >= m_memoryBegin.as_ptr && userPtr.as_ptr <= m_memoryEnd.as_ptr;
		assert(userPointerInAllocatorRange && "UserPointer was not allocared by this allocator. Not in memory range.");
	}

	// 1. Check whether the userPointer is really the last one allocated
	// 2. Read the total allocation size from the allocation meta
	// 3. Reset the current_ptr by the allocation size to free the mem
}

void sp::memory::StackAllocator::Reset()
{
	m_currentPtr = m_memoryBegin;
}

sp::memory::StackAllocator::~StackAllocator()
{
	if (m_usingInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin.as_void);
	}
}
