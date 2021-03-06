#pragma once

#include "MemoryRealmBase.h"
#include "Pointers/PointerUtil.h"

namespace sp
{
	namespace memory
	{
		/**
		 * The MemoryRealm is an abstraction over several strategies to allocate and debug memory
		 * allocations in this system. It takes two template type parameters, allowing to specify
		 * the allocation and bounds-checking strategy. These have to follow the common API in order
		 * to work with the memory realm. This allows for flexible creation of different allocators
		 * that can also change behaviour depending on the platform or build mode, if specified.
		 */
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
				const size_t totalMemory = BoundChecker::CANARY_SIZE + bytes + BoundChecker::CANARY_SIZE;

				char* memory = static_cast<char*>(m_allocator.Alloc(totalMemory, alignment, BoundChecker::CANARY_SIZE));

				m_boundsChecker.WriteCanary(memory);
				m_boundsChecker.WriteCanary(memory + m_boundsChecker.CANARY_SIZE + bytes);

				return memory + BoundChecker::CANARY_SIZE;
			}

			void Dealloc(void* memory) override
			{
				char* allocatorMemory = static_cast<char*>(memory) - BoundChecker::CANARY_SIZE;

				m_boundsChecker.ValidateFrontCanary(allocatorMemory);
				const uint32_t allocationSize = static_cast<uint32_t>(m_allocator.GetAllocationSize(allocatorMemory));
				m_boundsChecker.ValidateBackCanary(allocatorMemory + (allocationSize - BoundChecker::CANARY_SIZE));

				m_allocator.Dealloc(allocatorMemory);
			}

			void Reset(void) override
			{
				m_allocator.Reset();
			}

			~MemoryRealm() = default;

		private:
			Allocator m_allocator;
			BoundChecker m_boundsChecker;
		};
	}
}