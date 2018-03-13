#pragma once

class MemoryRealmBase
{
public:
	virtual void* Alloc(size_t bytes, size_t alignment) = 0;
	virtual void Dealloc(void* memory) = 0;
	virtual void Reset(void) = 0;
	virtual ~MemoryRealmBase() {}
};