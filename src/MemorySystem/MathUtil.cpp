#include "MathUtil.h"

uint32_t sp::math::RoundUp(uint32_t number, uint32_t multiple)
{
	if (multiple == 0)
	{
		return number;
	}

	const uint32_t remainder = number % multiple;
	
	if (remainder == 0)
	{
		return number;
	}

	return number + multiple - remainder;
}
