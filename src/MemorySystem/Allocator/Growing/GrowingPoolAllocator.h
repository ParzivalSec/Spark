#pragma once

#include <cstdint>

#include "../AllocatorBase.h"
#include "../../FreeList.h"

namespace sp
{
	namespace memory
	{
		class GrowingPoolAllocator : public AllocatorBase
		{
		public:
			GrowingPoolAllocator(size_t elementMaxSize, size_t elementCount, size_t elementCountMax, size_t elementMaxAlignment, size_t offset);

			void* Alloc(size_t size, size_t alignment, size_t offset) override;
			void Dealloc(void* memory) override;
			void Reset() override;

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
