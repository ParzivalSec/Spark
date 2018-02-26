#include "LinearAllocator.h"

#include <cassert>

#include "../../VirtualMemory/VirtualMemory.h"

///
/// Constructor taking an amount of memory the allocator should provide
/// Using this constructor the allocator internally allocates
/// a memory range via a virtual memory system on the platform
///
sp::memory::LinearAllocator::LinearAllocator(size_t size)
	: m_useInternalMemory(true)
{
	assert(size != 0 && "Cannot intialize an allocator with size 0");

	m_memoryBegin.as_void = ReserveAddressSpace(size);
	m_memoryBegin.as_void = CommitPhysicalMemory(m_memoryBegin.as_void, size);
	m_memoryEnd.as_ptr = m_memoryBegin.as_ptr + size;
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
	, m_memoryBegin { memoryStart }
	, m_memoryEnd { memoryEnd }
	, m_currentPtr { m_memoryBegin }
{
	bool isValidMemoryRange = memoryStart < memoryEnd;
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

	m_currentPtr.as_ptr += offset;
	m_currentPtr.as_void = pointerUtil::AlignTop(m_currentPtr.as_void, alignment);
	m_currentPtr.as_ptr -= offset;

	void* userPointer = m_currentPtr.as_void;
	
	m_currentPtr.as_ptr += size;

	if (m_currentPtr.as_ptr >= m_memoryEnd.as_ptr)
	{
		return nullptr;
	}

	return userPointer;
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

///
/// If the allocator has ownership over the memory
/// area, the destructor frees the physical and
/// virtual memory
///
sp::memory::LinearAllocator::~LinearAllocator()
{
	if (m_useInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin.as_void);
	}
}
