#include "StackAllocator.h"

#include <cassert>

#include "../../PointerUtil.h"
#include "../../VirtualMemory/VirtualMemory.h"

namespace
{
#ifdef STACK_ALLOC_LIFO_CHECKS
	struct AllocationHeader
	{
		uint32_t allocationOffset;
		uint32_t allocationSize;
		uint32_t allocationId;
	};

#else
	struct AllocationHeader
	{
		uint32_t allocationOffset;
		uint32_t allocationSize;
	};
#endif

	const uint32_t ALLOCATION_META_SIZE = sizeof(AllocationHeader);
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

	const ptrdiff_t allocationOffset = m_currentPtr - m_memoryBegin;
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
		AllocationHeader* as_header;
	};

	// Write the allocationOffset in the slot before the userPointer
	as_char = m_currentPtr;
	as_header->allocationOffset = static_cast<uint32_t>(allocationOffset);
	as_header->allocationSize = static_cast<uint32_t>(size);
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_header->allocationSize = ++m_allocationID;
#endif
	as_char += ALLOCATION_META_SIZE;
	m_currentPtr += size;

	return as_void;
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
		AllocationHeader* as_header;
	};

	as_void = memory;

	{
		const bool userPointerInAllocatorRange = as_char >= m_memoryBegin && as_char <= m_memoryEnd;
		assert(userPointerInAllocatorRange && "UserPointer was not allocared by this allocator. Not in memory range.");
	}

	as_char -= ALLOCATION_META_SIZE;

	const uint32_t allocationOffset = as_header->allocationOffset;
#ifdef STACK_ALLOC_LIFO_CHECKS
	{
		const uint32_t allocationID = as_header.allocationId;
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

size_t sp::memory::StackAllocator::GetAllocationSize(void* memory)
{
	{
		const bool isNotNull = memory != nullptr;
		assert(isNotNull && "Cannot return allocation size of a nullptr");
	}

	char* userPointer = static_cast<char*>(memory);
	return pointerUtil::pseudo_cast<AllocationHeader*>(userPointer - ALLOCATION_META_SIZE, 0)->allocationSize;
}

sp::memory::StackAllocator::~StackAllocator()
{
	if (m_usingInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin);
	}
}
