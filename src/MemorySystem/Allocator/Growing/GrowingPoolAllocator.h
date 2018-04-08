#pragma once

#include "../AllocatorBase.h"
#include "FreeList/FreeList.h"

namespace sp
{
	namespace memory
	{
		class GrowingPoolAllocator : public AllocatorBase
		{
		public:
			GrowingPoolAllocator(size_t elementMaxSize, size_t elementCount, size_t elementCountMax, size_t elementMaxAlignment, size_t offset);

			virtual void* Alloc(size_t size, size_t alignment, size_t offset) override;
			virtual void Dealloc(void* memory) override;
			virtual void Reset() override;

			virtual size_t GetAllocationSize(void* memory) override;

			inline ~GrowingPoolAllocator() override;

		private:
			char* m_virtualMemoryBegin;
			char* m_virtualMemoryEnd;
			char* m_physicalMemoryBegin;
			char* m_physicalMemoryEnd;
			char* m_firstChunkPtr;

			const size_t m_maxElementSize;
			const size_t m_maxElementAlignment;
			const size_t m_minimalChunkSize;
			const size_t m_growSize;

			core::FreeList m_freeList;
		};
	}
}
