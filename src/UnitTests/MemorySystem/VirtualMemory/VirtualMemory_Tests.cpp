#include "gtest/gtest.h"

#include "VirtualMemory/VirtualMemory.h"

///
/// Currently windows only tests
///

const size_t WINDOWS_PAGE_SIZE = 4096; // 4KB page size on windows
const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;
const size_t ONE_GIBIBYTE = 1024 * ONE_MIBIBYTE;

TEST(VirtualMemory, GetPageSize)
{
	size_t osPageSize = sp::memory::GetPageSize();
	ASSERT_EQ(osPageSize, WINDOWS_PAGE_SIZE);
}

TEST(VirtualMemory, ReserveRequestedAddressSpace)
{
	void* memBegin = sp::memory::ReserveAddressSpace(ONE_MIBIBYTE);
	ASSERT_NE(memBegin, nullptr) << "ReserveAddressSapce should not return a nullptr";
	sp::memory::FreeAddressSpace(memBegin);
}

TEST(VirtualMemory, FreeRequestedAddressSpace)
{
	void* memBegin = sp::memory::ReserveAddressSpace(ONE_MIBIBYTE);
	sp::memory::FreeAddressSpace(memBegin);
}

TEST(VirtualMemory, CommitPhysicalPages)
{
	void* memBegin = sp::memory::ReserveAddressSpace(ONE_MIBIBYTE);
	void* physicalMemBegin = sp::memory::CommitPhysicalMemory(memBegin, ONE_MIBIBYTE);
	ASSERT_NE(physicalMemBegin, nullptr) << "CommitPhysicalMemory should not return a nullptr";

	size_t source = 100;
	memcpy(physicalMemBegin, &source, sizeof(size_t));
	ASSERT_EQ(static_cast<size_t*>(physicalMemBegin)[0], 100) << "Could not read 100 from physical memory";

	sp::memory::FreeAddressSpace(memBegin);
}

TEST(VirtualMemory, CommitPhysicalPagesMedium)
{
	void* memBegin = sp::memory::ReserveAddressSpace(ONE_MIBIBYTE * 500);
	void* physicalMemBegin = sp::memory::CommitPhysicalMemory(memBegin, ONE_MIBIBYTE * 500);
	ASSERT_NE(physicalMemBegin, nullptr) << "CommitPhysicalMemory should not return a nullptr";

	size_t source = 100;
	memcpy(physicalMemBegin, &source, sizeof(size_t));
	ASSERT_EQ(static_cast<size_t*>(physicalMemBegin)[0], 100) << "Could not read 100 from physical memory";

	sp::memory::FreeAddressSpace(memBegin);
}

TEST(VirtualMemory, CommitPhysicalPagesLarge)
{
	void* memBegin = sp::memory::ReserveAddressSpace(ONE_GIBIBYTE);
	void* physicalMemBegin = sp::memory::CommitPhysicalMemory(memBegin, ONE_GIBIBYTE);
	ASSERT_NE(physicalMemBegin, nullptr) << "CommitPhysicalMemory should not return a nullptr";

	size_t source = 100;
	memcpy(physicalMemBegin, &source, sizeof(size_t));
	ASSERT_EQ(static_cast<size_t*>(physicalMemBegin)[0], 100) << "Could not read 100 from physical memory";

	sp::memory::FreeAddressSpace(memBegin);
}

TEST(VirtualMemory, DecommitPhysicalPages)
{
	void* memBegin = sp::memory::ReserveAddressSpace(ONE_MIBIBYTE);
	void* physicalMemBegin = sp::memory::CommitPhysicalMemory(memBegin, ONE_MIBIBYTE);
	sp::memory::DecommitPhysicalMemory(physicalMemBegin, ONE_MIBIBYTE);
	sp::memory::FreeAddressSpace(memBegin);
}