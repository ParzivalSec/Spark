#include "GrowingPoolAllocator.h"

#include <cassert>
#include <new>

#include "../../PointerUtil.h"
#include "../../VirtualMemory/VirtualMemory.h"
#include "../../MathUtil.h"

namespace
{
	static size_t CalculateMinimalChunkSize(size_t maxElementSize, size_t maxElementAlignment)
	{
		if (maxElementSize <= maxElementAlignment)
		{
			return maxElementAlignment;
		}
		else
		{
			return sp::math::RoundUp(maxElementSize, maxElementAlignment);
		}
	}

	static void* Grow(void* from, size_t size)
	{
		return sp::memory::CommitPhysicalMemory(from, size);
	}
}

sp::memory::GrowingPoolAllocator::GrowingPoolAllocator(size_t elementMaxSize, size_t elementCount,
	size_t elementCountMax, size_t elementMaxAlignment, size_t offset)
	: m_virtualMemoryBegin(nullptr)
	, m_virtualMemoryEnd(nullptr)
	, m_physicalMemoryBegin(nullptr)
	, m_physicalMemoryEnd(nullptr)
	, m_firstChunkPtr(nullptr)
	, m_maxElementSize(elementMaxSize)
	, m_maxElementAlignment(elementMaxAlignment)
	, m_minimalChunkSize(CalculateMinimalChunkSize(m_maxElementSize, m_maxElementAlignment))
	, m_growSize(sp::math::RoundUp((m_minimalChunkSize * elementCount) + m_maxElementAlignment, sp::memory::GetPageSize()))
{
	{
		const bool elementGreaterOrEqualPointerSize = elementMaxSize >= sizeof(uintptr_t);
		assert(elementGreaterOrEqualPointerSize && "Element size must at least be able to store a pointer on this system");
	}

	// Get memory to fulfill the request to store elementCount * elementSize objects with a maxAlignment of X
	// To do se we need to allocate X bytes more to be able to align top at least one time and fulfill the request however
	const size_t maximumMemorySize = (elementCountMax * m_minimalChunkSize) + m_maxElementAlignment;
	const size_t maximumMemorySizeRounded = sp::math::RoundUp(maximumMemorySize, sp::memory::GetPageSize());

	m_virtualMemoryBegin = pointerUtil::pseudo_cast<char*>(ReserveAddressSpace(maximumMemorySizeRounded), 0);
	m_virtualMemoryEnd = m_virtualMemoryBegin + maximumMemorySizeRounded;

	m_physicalMemoryBegin = pointerUtil::pseudo_cast<char*>(CommitPhysicalMemory(m_virtualMemoryBegin, m_growSize), 0);
	m_physicalMemoryEnd = m_physicalMemoryBegin + m_growSize;

	// Offset the first chunk and align it to ensure all following slots are also aligned
	m_firstChunkPtr = pointerUtil::AlignTop(m_physicalMemoryBegin + offset, m_maxElementAlignment) - offset;
	m_freeList.~FreeList();
	new (&m_freeList) core::FreeList(m_firstChunkPtr, m_physicalMemoryEnd, m_minimalChunkSize);
}

void* sp::memory::GrowingPoolAllocator::Alloc(size_t size, size_t alignment, size_t offset)
{
	{
		const bool sizeLesserOrEqualMaxElementSize = size <= m_maxElementSize;
		assert(sizeLesserOrEqualMaxElementSize && "Allocation size has to be lesser or equal to the maximum element size provided at construction");
		const bool alignmentLesserOrEqualMaxElementAlignment = alignment <= m_maxElementAlignment;
		assert(alignmentLesserOrEqualMaxElementAlignment && "Allocation alignment has to be lesser or equal to the maximum element alignment provided at construction");
	}

	if (m_freeList.IsEmpty())
	{
		{
			const bool canGrowFurther = m_physicalMemoryEnd + m_growSize <= m_virtualMemoryEnd;
			assert(canGrowFurther && "Growing pool allocator cannot grow further because virtual address space is exhausted");
		}

		char* newPhysicalMem = pointerUtil::pseudo_cast<char*>(Grow(m_physicalMemoryEnd, m_growSize), 0);
		void* newFirstChunkPtr = sp::pointerUtil::AlignTop(newPhysicalMem + offset, m_maxElementAlignment) - offset;
		m_physicalMemoryEnd = newPhysicalMem + m_growSize;

		m_freeList.~FreeList();
		new (&m_freeList) core::FreeList(newFirstChunkPtr, m_physicalMemoryEnd, m_minimalChunkSize);
	}

	return m_freeList.GetChunk();
}

void sp::memory::GrowingPoolAllocator::Dealloc(void* memory)
{
	{
		const bool isNotNull = memory != nullptr;
		assert(isNotNull && "Freeing a nullptr is not allowed");
		const bool isAllocatedFromAllocatorRange = memory >= m_physicalMemoryBegin && memory <= m_physicalMemoryEnd;
		assert(isAllocatedFromAllocatorRange && "Pointer was not allocated in the range of this allocator");
	}

	m_freeList.ReturnChunk(memory);
}

void sp::memory::GrowingPoolAllocator::Reset()
{
	m_freeList.~FreeList();
	new (&m_freeList) core::FreeList(m_firstChunkPtr, m_physicalMemoryEnd, m_minimalChunkSize);
}

sp::memory::GrowingPoolAllocator::~GrowingPoolAllocator()
{
	FreeAddressSpace(m_virtualMemoryBegin);
}
