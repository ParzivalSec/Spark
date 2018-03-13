#pragma once

#include "../AllocatorBase.h"

namespace sp
{
	namespace memory
	{
		class DoubleEndedStackAllocator : public AllocatorBase
		{
		public:
			DoubleEndedStackAllocator(size_t size);
			DoubleEndedStackAllocator(void* memoryBegin, void* memoryEnd);

			virtual void* Alloc(size_t size, size_t alignment, size_t offset) override;
			void* AllocBack(size_t size, size_t alignment, size_t offset);

			virtual void Dealloc(void* memory) override;
			void DeallocBack(void* memory);

			virtual void Reset() override;
			virtual size_t GetAllocationSize(void* memory) override;

			~DoubleEndedStackAllocator() override;

		private:
			bool m_usingInternalMemory;
			char* m_memoryBegin;
			char* m_memoryEnd;
			char* m_currentFront;
			char* m_currentBack;

#ifdef STACK_ALLOC_LIFO_CHECKS
			uint32_t m_frontAllocationId;
			uint32_t m_backAllocationId;
#endif
		};
	}
}