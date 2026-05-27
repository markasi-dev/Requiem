#pragma once

#include "defines.h"

typedef struct linear_allocator {
    u64 total_size;
    u64 allocated;
    void* memory;
    b8 owns_memory;
} linear_allocator;

RAPI void linear_allocator_create(u64 total_size, void* memory, linear_allocator* out_allocator);
RAPI void linear_allocator_destroy(linear_allocator* allocator);

RAPI void* linear_allocator_allocate(linear_allocator* allocator, u64 size);
RAPI void linear_allocator_free_all(linear_allocator* allocator);