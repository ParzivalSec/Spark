#include "PoolAllocator.h"

#include <new>

#include "../../PointerUtil.h"
#include "../../VirtualMemory/VirtualMemory.h"
#include "../../MathUtil.h"

namespace
{
	static size_t CalculateMinimalChunkSize(size_t maxElementSize, size_t maxElementAlignment)
	{
		if (maxElementSize < maxElementAlignment)
		{
			return maxElementAlignment;
		}
		else
		{
			return sp::math::RoundUp(maxElementSize, maxElementAlignment);
		}
	}
}


sp::memory::PoolAllocator::PoolAllocator(size_t elementMaxSize, size_t elementCount, size_t elementMaxAlignment, size_t offset)
	: m_useInternalMemory(true)
	, m_memoryBegin(nullptr)
	, m_memoryEnd(nullptr)
	, m_firstChunkPtr(nullptr)
	, m_maxElementSize(elementMaxSize)
	, m_maxElementAlignment(elementMaxAlignment)
	, m_minimalChunkSize(CalculateMinimalChunkSize(m_maxElementSize, m_maxElementAlignment))
{
	{
		const bool elementGreaterOrEqualPointerSize = elementMaxSize >= sizeof(uintptr_t);
		assert(elementGreaterOrEqualPointerSize && "Element size must at least be able to store a pointer on this system");
	}

	// Get memory to fulfill the request to store elementCount * elementSize objects with a maxAlignment of X
	// To do se we need to allocate X bytes more to be able to align top at least one time and fulfill the request however
	const size_t requiredMemorySize = (elementCount * m_minimalChunkSize) + m_maxElementAlignment;

	m_memoryBegin = pointerUtil::pseudo_cast<char*>(ReserveAddressSpace(requiredMemorySize), 0);
	m_memoryBegin = pointerUtil::pseudo_cast<char*>(CommitPhysicalMemory(m_memoryBegin, requiredMemorySize), 0);
	m_memoryEnd = m_memoryBegin + requiredMemorySize;

	// Offset the first chunk and align it to ensure all following slots are also aligned
	m_firstChunkPtr = pointerUtil::AlignTop(m_memoryBegin + offset, m_maxElementAlignment) - offset;
	m_freeList.~FreeList();
	new (&m_freeList) core::FreeList(m_firstChunkPtr, m_memoryEnd, m_minimalChunkSize);
}

sp::memory::PoolAllocator::PoolAllocator(void* memoryBegin, void* memoryEnd, size_t elementMaxSize, size_t elementMaxAlignment, size_t offset)
	: m_useInternalMemory(false)
	, m_memoryBegin(pointerUtil::pseudo_cast<char*>(memoryBegin, 0))
	, m_memoryEnd(pointerUtil::pseudo_cast<char*>(memoryEnd, 0))
	, m_firstChunkPtr(nullptr)
	, m_maxElementSize(elementMaxSize)
	, m_maxElementAlignment(elementMaxAlignment)
	, m_minimalChunkSize(CalculateMinimalChunkSize(m_maxElementSize, m_maxElementAlignment))
{
	{
		const bool elementGreaterOrEqualPointerSize = elementMaxSize >= sizeof(uintptr_t);
		assert(elementGreaterOrEqualPointerSize && "Element size must at least be able to store a pointer on this system");
	}

	const ptrdiff_t memoryBlockLength = m_memoryEnd - m_memoryBegin;

	// Offset the first chunk and align it to ensure all following slots are also aligned
	m_firstChunkPtr = pointerUtil::AlignTop(m_memoryBegin + offset, m_maxElementAlignment) - offset;
	m_freeList.~FreeList();
	new (&m_freeList) core::FreeList(m_firstChunkPtr, m_memoryEnd, m_minimalChunkSize);
}

void* sp::memory::PoolAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	{
		const bool sizeLesserOrEqualMaxElementSize = size <= m_maxElementSize;
		assert(sizeLesserOrEqualMaxElementSize && "Allocation size has to be lesser or equal to the maximum element size provided at construction");
		const bool alignmentLesserOrEqualMaxElementAlignment = alignment <= m_maxElementAlignment;
		assert(alignmentLesserOrEqualMaxElementAlignment && "Allocation alignment has to be lesser or equal to the maximum element alignment provided at construction");
	}

	return m_freeList.GetChunk();
}

void sp::memory::PoolAllocator::Dealloc(void* memory)
{
	{
		const bool isNotNull = memory != nullptr;
		assert(isNotNull && "Freeing a nullptr is not allowed");
		const bool isAllocatedFromAllocatorRange = memory >= m_memoryBegin && memory <= m_memoryEnd;
		assert(isAllocatedFromAllocatorRange && "Pointer was not allocated in the range of this allocator");
	}

	m_freeList.ReturnChunk(memory);
}

void sp::memory::PoolAllocator::Reset()
{
	m_freeList.~FreeList();
	new (&m_freeList) core::FreeList(m_firstChunkPtr, m_memoryEnd, m_minimalChunkSize);
}

sp::memory::PoolAllocator::~PoolAllocator()
{
	if (m_useInternalMemory)
	{
		FreeAddressSpace(m_memoryBegin);
	}
}
