#include "SimpleBoundsChecker.h"
#include "../PointerUtil.h"

const uint8_t sp::memory::SimpleBoundsChecker::CANARY_SIZE = 4;

void sp::memory::SimpleBoundsChecker::WriteCanary(void* const memory) const
{
	uint32_t* canaryLocation = sp::pointerUtil::pseudo_cast<uint32_t*>(memory, 0);
	*canaryLocation = m_canary;
}

bool sp::memory::SimpleBoundsChecker::ValidateCanary(void* const memory) const
{
	uint32_t* canaryLocation = sp::pointerUtil::pseudo_cast<uint32_t*>(memory, 0);
	return *canaryLocation == m_canary;
}
