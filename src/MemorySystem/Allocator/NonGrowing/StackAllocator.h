#pragma once

#include "../AllocatorBase.h"

namespace sp
{
	namespace memory
	{
		/*
		 * A non-growing stack-based allocator
		 * 
		 * The stack base allocator allows freeing single allocations in a last-in-first-out
		 * order. A LIFO check can be enabled via #define that checks the validity of that
		 * order on every deallocation by introducing a small overhead.
		 */
		class StackAllocator : public AllocatorBase
		{
		public:
			explicit StackAllocator(size_t size);
			StackAllocator(void* memoryStart, void* memoryEnd);

			StackAllocator(const StackAllocator& other) = delete;
			StackAllocator(const StackAllocator&& other) = delete;
			StackAllocator operator=(const StackAllocator& other) = delete;
			StackAllocator operator=(const StackAllocator&& other) = delete;

			virtual void* Alloc(size_t size, size_t alignment, size_t offset) override;
			virtual void Dealloc(void* memory) override;
			virtual void Reset() override;
			virtual size_t GetAllocationSize(void* memory) override;

			virtual ~StackAllocator() override;
		private:
			bool m_usingInternalMemory;
			char* m_memoryBegin;
			char* m_memoryEnd;
			char* m_currentPtr;

#ifdef STACK_ALLOC_LIFO_CHECKS
			uint32_t m_allocationID;
#endif
		};
	}
}
