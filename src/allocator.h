#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

// public api

void* alloc(size_t size);
void dealloc(void* ptr);
void* ralloc(void* ptr, size_t size);
void* rallocf(void* ptr, size_t size);
void* calloc_(size_t num, size_t size);

#endif /* ALLOCATOR_H */