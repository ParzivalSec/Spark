#include "MathUtil.h"

size_t sp::math::RoundUp(size_t number, size_t multiple)
{
	if (multiple == 0)
	{
		return number;
	}

	const size_t remainder = number % multiple;
	
	if (remainder == 0)
	{
		return number;
	}

	return number + multiple - remainder;
}
