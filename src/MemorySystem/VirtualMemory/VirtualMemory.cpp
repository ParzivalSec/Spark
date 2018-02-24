#include "VirtualMemory.h"

#include <windows.h>

namespace sp
{
	namespace memory
	{
		size_t GetPageSize(void)
		{
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			return sysInfo.dwPageSize;
		}

		void* ReserveAddressSpace(size_t size)
		{
			return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
		}

		void FreeAddressSpace(void* from)
		{
			VirtualFree(from, 0u, MEM_RELEASE);
		}

		void* CommitPhysicalMemory(void* from, size_t size)
		{
			return VirtualAlloc(from, size, MEM_COMMIT, PAGE_READWRITE);
		}

		void DecommitPhysicalMemory(void* from, size_t size)
		{
			VirtualFree(from, size, MEM_DECOMMIT);
		}
	}
}
