#pragma once

#include <cstdint>

#include "../AllocatorBase.h"
#include "../../FreeList.h"

namespace sp
{
	namespace memory
	{
		class PoolAllocator : public AllocatorBase
		{
		public:
			PoolAllocator(size_t elementMaxSize, size_t elementCount, size_t elementMaxAlignment, size_t offset);
			PoolAllocator(void* memoryBegin, void* memoryEnd, size_t elementMaxSize, size_t elementMaxAlignment, size_t offset);

			void* Alloc(size_t size, size_t alignment, size_t offset) override;
			void Dealloc(void* memory) override;
			void Reset() override;
			
			inline ~PoolAllocator() override;
		
		private:
			bool m_useInternalMemory;

			char* m_memoryBegin;
			char* m_memoryEnd;
			char* m_firstChunkPtr;

			const size_t m_maxElementSize;
			const size_t m_maxElementAlignment;
			const size_t m_minimalChunkSize;

			core::FreeList m_freeList;
		};
	}
}
