#include "gtest/gtest.h"

#include "Allocator/NonGrowing/LinearAllocator.h"
#include "BoundsChecker/SimpleBoundsChecker.h"
#include "MemoryRealm/MemoryRealm.h"
#include "PointerUtil.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;

typedef sp::memory::MemoryRealm<sp::memory::LinearAllocator, sp::memory::SimpleBoundsChecker> SimpleLinearRealm;
typedef sp::memory::MemoryRealm<sp::memory::LinearAllocator, sp::memory::SimpleBoundsChecker> SimpleLinearRealm_Retail;

TEST(SimpleMemoryRealm, Linear_Allocator_With_BoundsChecking_sets_canaries)
{
	SimpleLinearRealm memRealm(ONE_MIBIBYTE * 100);
	void* raw_mem_0 = memRealm.Alloc(ONE_MIBIBYTE, 1);

	ASSERT_TRUE(*sp::pointerUtil::pseudo_cast<uint32_t*>(raw_mem_0, -4) == 0xCA) << "No front canary set!";
	ASSERT_TRUE(*sp::pointerUtil::pseudo_cast<uint32_t*>(raw_mem_0, ONE_MIBIBYTE) == 0xCA) << "No back canary set!";
}

TEST(SimpleMemoryRealm, Linear_Allocator_With_BoundsChecking_Asserts_On_Front_Stomps)
{
	SimpleLinearRealm memRealm(ONE_MIBIBYTE * 100);
	void* raw_mem_0 = memRealm.Alloc(ONE_MIBIBYTE, 1);
	*sp::pointerUtil::pseudo_cast<uint32_t*>(raw_mem_0, -4) = 0xAA;

	ASSERT_DEATH(memRealm.Dealloc(raw_mem_0), "Front Canary was not valid");
}

TEST(SimpleMemoryRealm, Linear_Allocator_With_BoundsChecking_Asserts_On_Back_Stomps)
{
	SimpleLinearRealm memRealm(ONE_MIBIBYTE * 100);
	void* raw_mem_0 = memRealm.Alloc(ONE_MIBIBYTE, 1);
	*sp::pointerUtil::pseudo_cast<uint32_t*>(raw_mem_0, ONE_MIBIBYTE) = 0xAA;

	ASSERT_DEATH(memRealm.Dealloc(raw_mem_0), "Back Canary was not valid");
}