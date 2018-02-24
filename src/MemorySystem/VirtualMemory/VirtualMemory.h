#pragma once

namespace sp
{
	namespace memory
	{
		size_t GetPageSize(void);

		void* ReserveAddressSpace(size_t size);
		void FreeAddressSpace(void* from);

		void* CommitPhysicalMemory(void* from, size_t size);
		void DecommitPhysicalMemory(void* from, size_t size);
	}
}