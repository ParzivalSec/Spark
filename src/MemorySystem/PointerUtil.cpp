#include "PointerUtil.h"

bool sp::pointerUtil::IsPowerOfTwo(size_t number)
{
	// Bit pattern of POTs: 0100 => -1: 0011 => & them yields 0 for POTs
	return number && !(number & (number - 1));
}
