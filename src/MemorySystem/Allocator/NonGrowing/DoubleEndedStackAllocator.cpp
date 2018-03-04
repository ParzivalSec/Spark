#include "DoubleEndedStackAllocator.h"
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

sp::memory::DoubleEndedStackAllocator::DoubleEndedStackAllocator(size_t size)
	: m_usingInternalMemory(true)
	, m_memoryBegin(nullptr)
	, m_memoryEnd(nullptr)
	, m_currentFront(nullptr)
	, m_currentBack(nullptr)
#ifdef STACK_ALLOC_LIFO_CHECKS
	, m_frontAllocationId(0)
	, m_backAllocationId(0)
#endif
{
	assert(size != 0 && "Cannot intialize an allocator with size 0");

	m_memoryBegin = pointerUtil::pseudo_cast<char*>(ReserveAddressSpace(size), 0);
	m_memoryBegin = pointerUtil::pseudo_cast<char*>(CommitPhysicalMemory(m_memoryBegin, size), 0);
	m_memoryEnd = m_memoryBegin + size;
	
	m_currentFront = m_memoryBegin;
	m_currentBack = m_memoryEnd;
}

sp::memory::DoubleEndedStackAllocator::DoubleEndedStackAllocator(void* memoryBegin, void* memoryEnd)
	: m_usingInternalMemory(false)
	, m_memoryBegin(pointerUtil::pseudo_cast<char*>(memoryBegin, 0))
	, m_memoryEnd(pointerUtil::pseudo_cast<char*>(memoryEnd, 0))
	, m_currentFront(pointerUtil::pseudo_cast<char*>(memoryBegin, 0))
	, m_currentBack(pointerUtil::pseudo_cast<char*>(memoryEnd, 0))
#ifdef STACK_ALLOC_LIFO_CHECKS
	, m_frontAllocationId(0)
	, m_backAllocationId(0)
#endif
{
	const bool isValidMemoryRange = m_memoryBegin < m_memoryEnd;
	assert(isValidMemoryRange && "Memory end is not allowed to be lesser or equal than memory start");
}

void* sp::memory::DoubleEndedStackAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	assert(pointerUtil::IsPowerOfTwo(alignment) && "Alignment has to be a power-of-two");

	// INFO: Used a ptrdiff_t here earlier by this would take up 8 byte on 64bit machines
	// so I decided onto uint32_t and assume the allocator will never use more than 2^32
	// bytes (around 4GB allowed per allocator)
	const uint32_t allocationOffset = m_currentFront - m_memoryBegin;
	// Align the currentPtr offsetted by offset + AllocationMetaSize to not
	// mess up alignment when adding canaries later
	m_currentFront += offset + ALLOCATION_META_SIZE;
	m_currentFront = pointerUtil::pseudo_cast<char*>(pointerUtil::AlignTop(m_currentFront, alignment), 0);
	m_currentFront -= offset + ALLOCATION_META_SIZE;

	if (m_currentFront + size + ALLOCATION_META_SIZE > m_currentBack)
	{
		// TODO: Think about adding a debug log to notify when this happens
		return nullptr;
	}

	union
	{
		void* as_void;
		char* as_char;
		uint32_t* as_uint32_t;
	};

	// Write the allocationOffset in the slot before the userPointer
	as_char = m_currentFront;
	*as_uint32_t = allocationOffset;
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_char += sizeof(uint32_t);
	*as_uint32_t = ++m_frontAllocationId;
	as_char += sizeof(uint32_t);
#else
	as_char += ALLOCATION_META_SIZE;
#endif
	// Store the userPointer into a temp bc otherwise it would be corrupted by
	// advancing the currentPtr by size bytes
	void* userPtr = as_void;
	m_currentFront += size;
	return userPtr;
}

void* sp::memory::DoubleEndedStackAllocator::AllocBack(size_t size, size_t alignment, size_t offset)
{
	assert(pointerUtil::IsPowerOfTwo(alignment) && "Alignment has to be a power-of-two");

	// INFO: Used a ptrdiff_t here earlier by this would take up 8 byte on 64bit machines
	// so I decided onto uint32_t and assume the allocator will never use more than 2^32
	// bytes (around 4GB allowed per allocator)
	const uint32_t allocationOffset = m_currentBack - m_memoryEnd;
	// Align the currentPtr offsetted by offset + AllocationMetaSize to not
	// mess up alignment when adding canaries later
	m_currentBack -= size;
	m_currentBack = pointerUtil::pseudo_cast<char*>(pointerUtil::AlignBottom(m_currentBack, alignment), 0);
	m_currentBack -= (ALLOCATION_META_SIZE + offset);

	if (m_currentBack < m_currentFront)
	{
		// TODO: Think about adding a debug log to notify when this happens
		return nullptr;
	}

	union
	{
		void* as_void;
		char* as_char;
		uint32_t* as_uint32_t;
	};

	// Write the allocationOffset in the slot before the userPointer
	as_char = m_currentBack;
	*as_uint32_t = allocationOffset;
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_char += sizeof(uint32_t);
	*as_uint32_t = ++m_backAllocationId;
	as_char -= sizeof(uint32_t);
#else
	as_char += ALLOCATION_META_SIZE;
#endif
	// Store the userPointer into a temp bc otherwise it would be corrupted by
	// advancing the currentPtr by size bytes
	void* userPtr = as_void;
	m_currentBack -= ALLOCATION_META_SIZE;
	return userPtr;
}

void sp::memory::DoubleEndedStackAllocator::Dealloc(void* memory)
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
		const bool userPointerInFrontRange = as_char < m_currentBack;
		assert(userPointerInFrontRange && "UserPointer was not allocated from the allocator's front end");
	}

	as_char -= ALLOCATION_META_SIZE;

	const uint32_t allocationOffset = *as_uint32_t;
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_char += sizeof(uint32_t);
	{
		const uint32_t allocationID = *as_uint32_t;
		const bool wasFreedInLIFOFashion = allocationID == m_frontAllocationId;
		assert(wasFreedInLIFOFashion && "Freed other than last allocation by stack's front end. Stack allocator does only support freeing in LIFO order.");
		--m_frontAllocationId;
	}
#endif
	m_currentFront = m_memoryBegin + allocationOffset;
}

void sp::memory::DoubleEndedStackAllocator::DeallocBack(void* memory)
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
		const bool userPointerInBackRange = as_char >= m_currentBack;
		assert(userPointerInBackRange && "UserPointer was not allocated from the allocator's back end.");
	}

	as_char -= ALLOCATION_META_SIZE;

	const uint32_t allocationOffset = *as_uint32_t;
#ifdef STACK_ALLOC_LIFO_CHECKS
	as_char += sizeof(uint32_t);
	{
		const uint32_t allocationID = *as_uint32_t;
		const bool wasFreedInLIFOFashion = allocationID == m_backAllocationId;
		assert(wasFreedInLIFOFashion && "Freed other than last allocation by stack's back end. Stack allocator does only support freeing in LIFO order.");
		--m_backAllocationId;
	}
#endif
	m_currentBack = m_memoryEnd - allocationOffset;
}

void sp::memory::DoubleEndedStackAllocator::Reset()
{
	m_currentFront = m_memoryBegin;
	m_currentBack = m_memoryEnd;
}

sp::memory::DoubleEndedStackAllocator::~DoubleEndedStackAllocator()
{
	if (m_usingInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin);
	}
}