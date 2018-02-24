#include "VirtualMemory.h"

namespace sp
{
	namespace memory
	{
		size_t GetPageSize(void)
		{
			return 0;
		}

		void* ReserveAddressSpace(size_t size)
		{
			return nullptr;
		}

		void FreeAddressSpace(void* from, size_t size)
		{
			// TODO: Unimplemented function
		}

		void* CommitPhysicalMemory(void* from, size_t size)
		{
			return nullptr;
		}

		void DecommitPhysicalMemory(void* from, size_t size)
		{
			// TODO: Unimplemented function
		}
	}
}
