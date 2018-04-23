#pragma once

// Simple allocations without using the allocated memory
void allocate_100KB_unaligned_hundred_times();
void allocate_1MB_unaligned_hundred_times();
void allocate_50MB_unaligned_ten_times();

// Allocating space and constructing objects into it
void allocate_space_for_50_small_objects();
void allocate_space_for_50_medium_objects();
void allocate_space_for_50_large_objects();