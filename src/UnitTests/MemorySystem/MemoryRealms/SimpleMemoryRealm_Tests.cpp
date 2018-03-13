#include "gtest/gtest.h"

#include "BoundsChecker/SimpleBoundsChecker.h"
#include "PointerUtil.h"
#include "MemoryRealm/MemoryRealm.h"
#include "Allocator/NonGrowing/LinearAllocator.h"

const size_t ONE_KIBIBYTE = 1024;
const size_t ONE_MIBIBYTE = 1024 * ONE_KIBIBYTE;

typedef sp::memory::MemoryRealm<sp::memory::LinearAllocator, sp::memory::SimpleBoundsChecker> SimpleLinearRealm;
typedef sp::memory::MemoryRealm<sp::memory::LinearAllocator, sp::memory::SimpleBoundsChecker> SimpleLinearRealm_Retail;

TEST(SimpleMemoryRealm, Linear_Allocator_With_BoundsChecking)
{
	SimpleLinearRealm memRealm(ONE_MIBIBYTE * 100);
	void* raw_mem_0 = memRealm.Alloc(ONE_MIBIBYTE, 1);

	union
	{
		void* as_void;
		char* as_char;
		uint32_t* as_uint32_t;
	};

	as_void = raw_mem_0;
	ASSERT_TRUE(*sp::pointerUtil::pseudo_cast<uint32_t*>(as_char, -4) == 0xCA) << "No front canary set!";
	ASSERT_TRUE(*sp::pointerUtil::pseudo_cast<uint32_t*>(as_char + ONE_MIBIBYTE, 0) == 0xCA) << "No back canary set!";
}