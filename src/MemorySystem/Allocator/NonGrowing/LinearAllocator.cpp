#include "LinearAllocator.h"

#include <cassert>

#include "../../VirtualMemory/VirtualMemory.h"

namespace
{
	struct AllocationHeader
	{
		uint32_t allocationSize;
	};

	const uint32_t ALLOCATION_META_SIZE = sizeof(AllocationHeader);
}

///
/// Constructor taking an amount of memory the allocator should provide
/// Using this constructor the allocator internally allocates
/// a memory range via a virtual memory system on the platform
///
sp::memory::LinearAllocator::LinearAllocator(size_t size)
	: m_useInternalMemory(true)
{
	assert(size != 0 && "Cannot intialize an allocator with size 0");

	m_memoryBegin = static_cast<char*>(ReserveAddressSpace(size));
	m_memoryBegin = static_cast<char*>(CommitPhysicalMemory(m_memoryBegin, size));
	m_memoryEnd = m_memoryBegin + size;
	m_currentPtr = m_memoryBegin;
}

///
/// Constructor using a user provided area of memory
/// The allocator does not take ownership over the memory
/// and therefore the user is responsible for properly
/// free it when not used anymore
///
sp::memory::LinearAllocator::LinearAllocator(void* memoryStart, void* memoryEnd)
	: m_useInternalMemory(false)
	, m_memoryBegin(pointerUtil::pseudo_cast<char*>(memoryStart, 0))
	, m_memoryEnd(pointerUtil::pseudo_cast<char*>(memoryEnd, 0))
	, m_currentPtr(m_memoryBegin)
{
	const bool isValidMemoryRange = memoryStart < memoryEnd;
	assert(isValidMemoryRange && "Memory end is not allowed to be lesser or equal than memory start");
}

///
/// The Alloc() method of this allocator simply performans a pointer bump to advance
/// an internal pointer though the memory region. 
/// Alignment is mandatory to be a power-of-two and the offset can be used to 
/// properly align blocks with canaries or similar (combined usage with realms)
///
void* sp::memory::LinearAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	assert(pointerUtil::IsPowerOfTwo(alignment) && "Alignment has to be a power-of-two");

	m_currentPtr += offset + ALLOCATION_META_SIZE;
	m_currentPtr = static_cast<char*>(pointerUtil::AlignTop(m_currentPtr, alignment));
	m_currentPtr -= offset + ALLOCATION_META_SIZE;

	union
	{
		void* as_void;
		char* as_char;
		AllocationHeader* as_header;
	};

	as_void = m_currentPtr;
	as_header->allocationSize = static_cast<uint32_t>(size);
	as_char += ALLOCATION_META_SIZE;
	
	m_currentPtr += size;

	if (m_currentPtr >= m_memoryEnd)
	{
		return nullptr;
	}

	return as_void;
}

///
/// The allocator does not support freeing single allocations
///
void sp::memory::LinearAllocator::Dealloc(void* memory) {}

///
/// The allocator sets the internal pointer to point
/// to the beginning of the memory block, allowing
/// for overwritting obsolete allocations by new ones
///
void sp::memory::LinearAllocator::Reset()
{
	m_currentPtr = m_memoryBegin;
}

size_t sp::memory::LinearAllocator::GetAllocationSize(void* memory)
{
	{
		const bool isNotNull = memory != nullptr;
		assert(isNotNull && "Cannot return allocation size of a nullptr");
	}

	char* userPointer = static_cast<char*>(memory);
	return pointerUtil::pseudo_cast<AllocationHeader*>(userPointer - ALLOCATION_META_SIZE, 0)->allocationSize;
}

///
/// If the allocator has ownership over the memory
/// area, the destructor frees the physical and
/// virtual memory
///
sp::memory::LinearAllocator::~LinearAllocator()
{
	if (m_useInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin);
	}
}
