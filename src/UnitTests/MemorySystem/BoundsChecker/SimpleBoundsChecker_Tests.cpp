#include "gtest/gtest.h"

#include "BoundsChecker/SimpleBoundsChecker.h"
#include "PointerUtil.h"

TEST(Simple_BoundsChecker, Write_Canary_To_Location)
{
	sp::memory::SimpleBoundsChecker boundsChecker;
	char* raw_mem = new char[16];

	boundsChecker.WriteCanary(raw_mem);
	boundsChecker.WriteCanary(raw_mem + 8);

	const uint32_t valueAtFrontCanaryLocation = *sp::pointerUtil::pseudo_cast<uint32_t*>(raw_mem, 0);
	const uint32_t valueAtBackCanaryLocation = *sp::pointerUtil::pseudo_cast<uint32_t*>(raw_mem, 8);

	ASSERT_TRUE(valueAtFrontCanaryLocation == 0xCA) << "Front canary was not properly written or corrupted";
	ASSERT_TRUE(valueAtBackCanaryLocation == 0xCA) << "Back canary was not properly written or corrupted";
}

TEST(Simple_BoundsChecker, Return_False_On_Corrupted_Back_Canary)
{
	sp::memory::SimpleBoundsChecker boundsChecker;
	char* raw_mem = new char[16];

	boundsChecker.WriteCanary(raw_mem);
	boundsChecker.WriteCanary(raw_mem + 8);

	char* userPtr = raw_mem + 4;

	struct BiggerThanEight
	{
		bool before_a;
		uint64_t a;
	};

	const BiggerThanEight someVariable { true, 1 };

	BiggerThanEight* somePtr = sp::pointerUtil::pseudo_cast<BiggerThanEight*>(userPtr, 0);
	*somePtr = someVariable;

	ASSERT_DEATH(boundsChecker.ValidateBackCanary(raw_mem + 8), "Back Canary was not valid") << "Could not validate back canary";
}

TEST(Simple_BoundsChecker, Return_False_On_Corrupted_Front_Canary)
{
	sp::memory::SimpleBoundsChecker boundsChecker;
	char* raw_mem = new char[16];

	boundsChecker.WriteCanary(raw_mem);
	boundsChecker.WriteCanary(raw_mem + 8);

	char* userPtr = raw_mem;

	struct BiggerThanEight
	{
		bool before_a;
		uint64_t a;
	};

	const BiggerThanEight someVariable{ true, 1 };

	BiggerThanEight* somePtr = sp::pointerUtil::pseudo_cast<BiggerThanEight*>(userPtr, 0);
	*somePtr = someVariable;

	ASSERT_DEATH(boundsChecker.ValidateFrontCanary(raw_mem), "Front Canary was not valid") << "Could not validate front canary";
}