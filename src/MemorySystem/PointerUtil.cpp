#include "PointerUtil.h"

void* sp::pointerUtil::AlignTop(void* pointer, size_t alignment)
{
	PointerType ptr { pointer };

	ptr.as_ptr += (alignment - 1);
	ptr.as_ptr &= ~(alignment - 1);

	return ptr.as_void;
}

void* sp::pointerUtil::AlignBottom(void* pointer, size_t alignment)
{
	PointerType ptr{ pointer };
	ptr.as_ptr &= ~(alignment - 1);
	return ptr.as_void;
}

bool sp::pointerUtil::IsPowerOfTwo(size_t number)
{
	// Bit pattern of POTs: 0100 => -1: 0011 => & them yields 0 for POTs
	return number && !(number & (number - 1));
}
