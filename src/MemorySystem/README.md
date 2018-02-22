# Memory Management

This folder is the root directory of the module that contains everything related to the memory management sub-module. Because todays games are high-performant pieces of software memory management and proper control is mandatory for a good game engine. This sub-module implements common techniques of engines to serve this purpose. 

## API Design

### MemoryRealms

A MemoryRealm combines several strategies on how memory can be allocated. It knows about at least the following principles:

- How the memory is allocated/retrieved (also referred to as the AllocationStrategy) - Linear, stack-fashion, pooled, ...
- Bounds checking strategy (also BoundCheckStrategy) - none at all, canaries, ...
- Memory tracking strategy (also MemTrackingStrategy) - none, simple counters, filename + line, ...

The realm then knows, based on the provided strategies, how it shall process an allocation request done by the client. To show how such a process can look the following situation describes this:

- User created a MemoryRealm and configured it to use the following strategies
    - LinearAllocationStrategy (allocation via pointer bump, no single free, only whole memory block at once)
    - SimpleBoundCheckStrategy (allocations have front and back canaries to discover memory stomps)
    - EmptyMemTrackingStrategy (no tracking of the allocations)

- The user then allocates X bytes of memory and uses the method of the realm that does not expect an alignment constraint
- Internally the realm then passes authority to the bounds-checker configured to decide if the allocation has to become bigger due to additional meta-data
- Then the allocator provides the memory requested by the user + the additional space required by the bounds-checker
- Then again the bounds-checker has it's turn to write the canaries to the front/back of the user's memory block
- Bc the mem-tracker is configured to be empty no information about the allocation is tracked
- The realm returns the pointer to the mem block (offsetted by the canary size) to the user

The described situation shows how a MemoryRealm with a linear allocator and bounds-checking would work. Due to the design of the realm and the fact that is passes authority to the strategies themselves it is now possible to create a variety of different realms fitting different situations and requirements.

### Allocator strategies

Spark++ will feature the following allocation strategies that can be configured for a realm:

- Linear allocator (pointer bump / no single free / whole allocator reset)
- Stack allocator (LIFO allocs / single free, but LIFO)
- Double-ended stack allocator (Fill stack alloc from both ends, LIFO allocs, single free LIFO)
- Pool allocator (objects with same size / alloc & free in O(1) / free-list book-keeping internals)
- Growing pool allocator ( ---"--- / grows when mem is exhausted)
- ?General purpose alloctor (maybe but not sure / maybe an external one)

Allocator strategy details can be found in the readme's in the folder of the strategy

### Bound checking strategies

Spark++ will feature the following mechanisms for bounds checking:

- Canaries (set canaries at the front/back of memory blocks to discover stomps on free)
- Protected pages (alloc an additional mem page and set it protected / discovers stomps when they occur but costly)

### Mem tracking strategies

Spark++ will also implement memory tracking strategies to profile the memory access/usage of the application with:

- Simple counter (increase on alloc() and decrease on free() / can show if a leak happened)
- Mem logger (logs file, line, ... info per allocation / mem patterns can be tracked)
- ?Stacktrace (gathers the stacktrace at the allocation point and store this)