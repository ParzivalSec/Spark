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
