#include "StackAllocator.h"

#include <cassert>

#include "../../PointerUtil.h"
#include "../../VirtualMemory/VirtualMemory.h"

namespace
{
#ifdef STACK_ALLOC_LIFO_CHECKS
	const uint32_t ALLOCATION_META_SIZE = sizeof(uint64_t);
#else
	const uint32_t ALLOCATION_META_SIZE = sizeof(uint32_t);
#endif
}

sp::memory::StackAllocator::StackAllocator(size_t size)
	: m_usingInternalMemory(true)
	, m_memoryBegin(nullptr)
	, m_memoryEnd(nullptr)
	, m_currentPtr(nullptr)
#if defined(STACK_ALLOC_LIFO_CHECKS)
	, m_allocationID(0)
#endif
{
	assert(size != 0 && "Cannot intialize an allocator with size 0");

	m_memoryBegin = pointerUtil::pseudo_cast<char*>(ReserveAddressSpace(size), 0);
	m_memoryBegin = pointerUtil::pseudo_cast<char*>(CommitPhysicalMemory(m_memoryBegin, size), 0);
	m_memoryEnd = m_memoryBegin + size;
	m_currentPtr = m_memoryBegin;
}

sp::memory::StackAllocator::StackAllocator(void* memoryStart, void* memoryEnd)
	: m_usingInternalMemory(false)
	, m_memoryBegin(static_cast<char*>(memoryStart))
	, m_memoryEnd(static_cast<char*>(memoryEnd))
	, m_currentPtr(m_memoryBegin)
#if defined(STACK_ALLOC_LIFO_CHECKS)
	, m_allocationID(0)
#endif
{
	const bool isValidMemoryRange = memoryStart < memoryEnd;
	assert(isValidMemoryRange && "Memory end is not allowed to be lesser or equal than memory start");
}

void* sp::memory::StackAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	assert(pointerUtil::IsPowerOfTwo(alignment) && "Alignment has to be a power-of-two");

	// INFO: Used a ptrdiff_t here earlier by this would take up 8 byte on 64bit machines
	// so I decided onto uint32_t and assume the allocator will never use more than 2^32
	// bytes (around 4GB allowed per allocator)
	const uint32_t allocationOffset = m_currentPtr - m_memoryBegin;
	// Align the currentPtr offsetted by offset + AllocationMetaSize to not
	// mess up alignment when adding canaries later
	m_currentPtr += offset + ALLOCATION_META_SIZE;
	m_currentPtr = static_cast<char*>(pointerUtil::AlignTop(m_currentPtr, alignment));
	m_currentPtr -= offset + ALLOCATION_META_SIZE;

	if (m_currentPtr + size + ALLOCATION_META_SIZE > m_memoryEnd)
	{
		return nullptr;
	}

	union
	{
		void* as_void;
		char* as_char;
		uint32_t* as_uint32_t;
	};

	// Write the allocationOffset in the slot before the userPointer
	as_char = m_currentPtr;
	*as_uint32_t = allocationOffset;
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_char += sizeof(uint32_t);
	*as_uint32_t = ++m_allocationID;
	as_char += sizeof(uint32_t);
#else
	as_char += ALLOCATION_META_SIZE;
#endif
	// Store the userPointer into a temp bc otherwise it would be corrupted by
	// advancing the currentPtr by size bytes
	void* userPtr = as_void;
	m_currentPtr += size;
	return userPtr;
}

void sp::memory::StackAllocator::Dealloc(void* memory)
{
	{
		const bool isNotNull = memory != nullptr;
		assert(isNotNull && "Memory shall not be a nullptr");
	}

	union
	{
		void* as_void;
		char* as_char;
		uint32_t* as_uint32_t;
	};

	as_void = memory;

	{
		const bool userPointerInAllocatorRange = as_char >= m_memoryBegin && as_char <= m_memoryEnd;
		assert(userPointerInAllocatorRange && "UserPointer was not allocared by this allocator. Not in memory range.");
	}

	as_char -= ALLOCATION_META_SIZE;

	const uint32_t allocationOffset = *as_uint32_t;
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_char += sizeof(uint32_t);
	{
		const uint32_t allocationID = *as_uint32_t;
		const bool wasFreedInLIFOFashion = allocationID == m_allocationID;
		assert(wasFreedInLIFOFashion && "Freed other than last allocation. Stack allocator does only support freeing in LIFO order.");
		--m_allocationID;
	}
#endif
	m_currentPtr = m_memoryBegin + allocationOffset;
}

void sp::memory::StackAllocator::Reset()
{
	m_currentPtr = m_memoryBegin;
}

sp::memory::StackAllocator::~StackAllocator()
{
	if (m_usingInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin);
	}
}
