#include "allocator.h"
/* mmap only on linux */
#include <sys/mman.h>

#define align8(x) (((x) + 7) & ~7)
#define ALLOCABLE_SIZE sizeof(struct Block)

static void* base = NULL;

/**
 * @brief block metadata.
 */
typedef struct Block {
    struct Block* next;
    struct Block* prev;
    size_t size;
    /* not an int because of 8 alignment */
    size_t free;
    /* pointer to the allocated block */
    void *ptr;
    char data[];
} Block;

/**
 * @brief Copies data from one block to another in 8 byte chunks.
 */
static void copyBlock(Block* source, Block* dest){
    size_t *sdata, *ddata;
    sdata = source->ptr;
    ddata = dest->ptr;
    for(size_t i = 0; i*8<source->size && i*8<dest->size; ++i){
        ddata[i] = sdata[i];
    }
}

/**
 * @brief Retrieves metadata from a data pointer.
 */
static Block* getBlock(void* ptr){
    return (Block*)((char*)ptr - ALLOCABLE_SIZE);
}

/**
 * @brief Checks if pointer was returned by alloc().
 */
static int validAddress(void* ptr){
    Block* block = getBlock(ptr);
    return block->ptr == block->data;
}

/**
 * @brief Merges a block with hist sucessor if both are free and from the same mmap() call.
 */
static Block* fusion(Block* block){
    if(block->next && block->next->free){
        /* 
        Because memory coming from different mmaps are not contiguous,
        fusion can only be when two blocks are from the same mmap().
        */
        if((char*)block->data + block->size == (char*)block->next){
            block->size += ALLOCABLE_SIZE + block->next->size;
            block->next = block->next->next;
            if(block->next){
                block->next->prev = block;
            }
        }
    }
    return block;
}

/**
 * @brief This function returns a pointer to a fitting chunk of memory,
 * or NULL if no suitable chunk was found.
 */
static Block* findAlloc(Block** last, size_t size){
    Block* block = base;
    while(block && !(block->free && block->size >= size)){
        *last = block;
        block = block->next;
    }
    return block;
}

/**
 * @brief This function splits a chunk of memory into two parts: one part of the requested size,
 * and another part with the remaining size. The first part is marked as allocated,
 * and the second part is marked as free.
 */
static void splitBlock(Block* block, size_t size){
    Block* newBlock = (Block*)(block->data + size);
    newBlock->size = block->size - size - ALLOCABLE_SIZE;
    newBlock->next = block->next;
    newBlock->prev = block;
    newBlock->ptr = newBlock->data;
    newBlock->free = 1;
    block->size = size;
    block->next = newBlock;
    if(newBlock->next){
        newBlock->next->prev = newBlock;
    }
}

/**
 * @brief This function extends the heap with a new chunk of memory and returns a pointer to it.
 * If the heap cannot be extended, it returns NULL.
 */
static Block* extendHeap(Block* last, size_t size){
    Block* block;
    block = mmap(NULL, ALLOCABLE_SIZE + size,
                PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_PRIVATE,
                -1, 0);
    if(block == MAP_FAILED){
        return NULL;
    }

    block->size = size;
    block->next = NULL;
    block->prev = last;
    block->ptr = block->data;
    block->free = 0;

    if(last){
        last->next = block;
    }
    return block;
}

/**
 * @brief This function allocates a chunk of memory of the given size and returns a pointer to it.
 */
void* alloc(size_t size){
    Block *block, *last;
    size_t s;
    s = align8(size);
    if (base) {
        last = base;
        block = findAlloc(&last, s);
        if (block) {
            if ((block->size - s) >= (ALLOCABLE_SIZE + 8))
                splitBlock(block, s);
            block->free = 0;
        } else {
            block = extendHeap(last, s);
            if (!block)
                return NULL;
        }
    } else {
        block = extendHeap(NULL, s);
        if (!block)
            return NULL;
        base = block;
    }
    return block->data;
}

/**
 * @brief This function marks a chunk of memory as free.
 * sbrk does provide all memory contiguously, but mmap does not,
 * so we cannot merge adjacent free chunks. Instead we just mark the chunk as free
 * and leave it to be reused by future allocations. 
 */
void free(void* ptr){
    if(validAddress(ptr)){
        Block* block = getBlock(ptr);
        block->free = 1;
        if(block->next){
            fusion(block);
        }
    }
}

/**
 * @brief allocates zero-initialized memory for an arr of elements.
 */
void *calloc(size_t num, size_t size){
    void *new = alloc(num * size);
    if(new){
        size_t s8 = align8(num * size) >> 3;
        for(size_t i = 0; i < s8; ++i){
            ((size_t*)new)[i] = 0;
        }
    }
    return new;
}

/**
 * @brief Resices allocated memory to a new size.
 */
void* realloc(void* ptr, size_t size){
    if(!ptr){
        return alloc(size);
    }
    if(validAddress(ptr)){
        size_t s = align8(size);
        Block* block = getBlock(ptr);
        if(block->size >= s){
            if(block->size - s >= ALLOCABLE_SIZE + 8){
                splitBlock(block, s);
            }
        }else {
            if(block->next && block->next->free &&
                (char*)block->data + block->size == (char*)block->next &&
                block->size + ALLOCABLE_SIZE + block->next->size >= s){
                    fusion(block);
                    if(block->size - s >= ALLOCABLE_SIZE + 8){
                        splitBlock(block, s);
                    }
                } else{
                    void* newptr = alloc(s);
                    if(!newptr) return NULL;
                    Block* new = getBlock(newptr);
                    copyBlock(block, new);
                    free(ptr);
                    return newptr;
                }
        }
        return ptr;
    }
    return NULL;
}

/**
 * @brief Resizes a block, freeing the original block if the new size cannot be allocated.
 */
void* reallocf(void* ptr, size_t size){
    void* newptr = realloc(ptr, size);
    if(!newptr){
        free(ptr);
    }
    return newptr;
}