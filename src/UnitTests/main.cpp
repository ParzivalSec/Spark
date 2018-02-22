#include "gtest/gtest.h"
#include "Allocators/Alloc.h"

TEST (MemorySystem, AllocNullptr)
{
	size_t b = 12;
	ASSERT_EQ(alloc(12), &b) << "alloc() did not return a nullptr";
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
