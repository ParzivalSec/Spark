#pragma once

///
/// The AllocatorBase interface is used to share an allocator
/// between multiple users and to define the minimal API an
/// allocator has to implement. Most of the time the overhead
/// of calling the virtual function can be ignored bc the 
/// allocator is used by it's concrete type. However, it shall
/// not restrict the user to use an allocator in a polymorphic
/// way.
///
class AllocatorBase
{
public:
	virtual void* Alloc(size_t size, size_t alignment, size_t offset) = 0;
	virtual void Dealloc(void* memory) = 0;
	virtual void Reset() = 0;
	virtual size_t GetAllocationSize(void* memory) = 0;
	virtual ~AllocatorBase() = 0;
};

// here to satisfy linker/standard
inline AllocatorBase::~AllocatorBase() {}

