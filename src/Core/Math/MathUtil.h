#pragma once

#include <cstdint>

namespace sp
{
	namespace math
	{
		size_t RoundUp(size_t number, size_t multiple);
		size_t RoundDown(size_t number, size_t multiple);
	}
}