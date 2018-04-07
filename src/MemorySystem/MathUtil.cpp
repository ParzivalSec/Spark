#include "MathUtil.h"

size_t sp::math::RoundUp(size_t number, size_t multiple)
{
	const size_t remainder = number % multiple;
	return number - remainder + multiple * !!(remainder);
}

size_t sp::math::RoundDown(size_t numToRound, size_t multiple)
{
	const size_t remainder = numToRound % multiple;
	return numToRound - remainder * !!(remainder);
}
