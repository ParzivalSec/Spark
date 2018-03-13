#pragma once

#include "MemoryRealmBase.h"
#include "../PointerUtil.h"

namespace sp
{
	namespace memory
	{
		template <typename Allocator, typename BoundChecker>
		class MemoryRealm : public MemoryRealmBase
		{
		public:
			explicit MemoryRealm(size_t bytes)
				: m_allocator(bytes)
			{}

			template<typename MemoryProvider>
			explicit MemoryRealm(MemoryProvider& memoryProvider)
				: m_allocator(memoryProvider.start(), memoryProvider.end())
			{}

			void* Alloc(size_t bytes, size_t alignment) override
			{
				const size_t totalMemory = m_boundsChecker.CANARY_SIZE + bytes + m_boundsChecker.CANARY_SIZE;

				char* memory = static_cast<char*>(m_allocator.Alloc(totalMemory, alignment, m_boundsChecker.CANARY_SIZE));

				m_boundsChecker.WriteCanary(memory);
				m_boundsChecker.WriteCanary(memory + m_boundsChecker.CANARY_SIZE + bytes);

				return memory + m_boundsChecker.CANARY_SIZE;
			}

			void Dealloc(void* memory) override
			{
				char* allocatorMemory = static_cast<char*>(memory) - m_boundsChecker.CANARY_SIZE;

				m_boundsChecker.ValidateFrontCanary(allocatorMemory);
				// TODO: Somehow we need to get the allocation size to check back canaries
				m_boundsChecker.ValidateBackCanary(allocatorMemory /* + allocationSize + CANARY_SIZE */);

				m_allocator.Dealloc(allocatorMemory);
			}

			void Reset(void) override
			{
				m_allocator.Reset();
			}

			~MemoryRealm() override {}

		private:
			Allocator m_allocator;
			BoundChecker m_boundsChecker;
		};
	}
}