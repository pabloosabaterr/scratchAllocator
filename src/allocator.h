#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

// public api

void* alloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* reallocf(void* ptr, size_t size);
void* calloc(size_t num, size_t size);

#endif /* ALLOCATOR_H */