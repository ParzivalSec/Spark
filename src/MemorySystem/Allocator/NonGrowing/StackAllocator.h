#pragma once
#include "../AllocatorBase.h"
#include "../../PointerUtil.h"

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
			pointerUtil::PointerType m_memoryBegin;
			pointerUtil::PointerType m_memoryEnd;
			pointerUtil::PointerType m_currentPtr;
		};
	}
}
