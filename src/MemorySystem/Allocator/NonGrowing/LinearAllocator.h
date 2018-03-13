#pragma once

#include "../AllocatorBase.h"
#include "../../PointerUtil.h"

///
/// A non-growing linear allocator
/// When allocating memory it just performans a pointer bump
/// to allocator for the user. There is no possibility to 
/// free a single allocation. Only the whole allocator can
/// be reset.
///
namespace sp
{
	namespace memory
	{
		class LinearAllocator : public AllocatorBase
		{
		public:
			explicit LinearAllocator(size_t size);
			LinearAllocator(void* memoryStart, void* memoryEnd);

			virtual void* Alloc(size_t size, size_t alignment, size_t offset) override;
			virtual void  Dealloc(void* memory) override;
			virtual void  Reset() override;
			virtual size_t GetAllocationSize(void* memory) override;

			virtual ~LinearAllocator() override;

		private:
			bool m_useInternalMemory;
			char* m_memoryBegin;
			char* m_memoryEnd;
			char* m_currentPtr;
		};
	}
}