#pragma once

#include <cstdint>

#include "../AllocatorBase.h"

#define STACK_ALLOC_LIFO_CHECKS

namespace sp
{
	namespace memory
	{
		class StackAllocator : public AllocatorBase
		{
		public:
			explicit StackAllocator(size_t size);
			StackAllocator(void* memoryStart, void* memoryEnd);

			virtual void* Alloc(size_t size, size_t alignment, size_t offset) override;
			virtual void Dealloc(void* memory) override;
			virtual void Reset() override;

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
