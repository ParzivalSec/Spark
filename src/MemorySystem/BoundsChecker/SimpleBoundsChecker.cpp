#include "SimpleBoundsChecker.h"
#include "../PointerUtil.h"

const uint8_t sp::memory::SimpleBoundsChecker::CANARY_SIZE = 4;

void sp::memory::SimpleBoundsChecker::WriteCanary(void* const memory) const
{
	uint32_t* canaryLocation = sp::pointerUtil::pseudo_cast<uint32_t*>(memory, 0);
	*canaryLocation = m_canary;
}

void sp::memory::SimpleBoundsChecker::ValidateFrontCanary(void* const memory) const
{
	uint32_t* canaryLocation = sp::pointerUtil::pseudo_cast<uint32_t*>(memory, 0);
	bool isValidCanary = *canaryLocation == m_canary;
	assert(isValidCanary && "Front Canary was not valid");  
}

void sp::memory::SimpleBoundsChecker::ValidateBackCanary(void* const memory) const
{
	uint32_t* canaryLocation = sp::pointerUtil::pseudo_cast<uint32_t*>(memory, 0);
	bool isValidCanary = *canaryLocation == m_canary;
	assert(isValidCanary && "Back Canary was not valid");
}
