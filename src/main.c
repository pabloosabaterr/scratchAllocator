#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>

#define align4(x) (((x) + 3) & ~3)



typedef struct Allocable {
    struct Allocable* next;
    size_t size;
    int freed;
} Allocable;

void* allocate(size_t size){
    size_t totalSize = sizeof(Allocable) + size;
    Allocable* ptr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(ptr == MAP_FAILED){
        return NULL;
    }

    ptr->size = size;
    ptr->next = NULL;

    return (void*)(ptr + 1);
}

void freeAlloc(void* ptr){
    Allocable* allocPtr = (Allocable*)ptr - 1;
    munmap(ptr, allocPtr->size);
}

void* reallocate(void* ptr, size_t newSize){
    assert(0  && "Not implemented yet");
    return NULL;
}

int main(){
    int* ptr = allocate(20 * sizeof(int));
    for(int i = 0; i < 20; i++){
        ptr[i] = i;
    }
    for(int i = 0; i < 20; i++){
        printf("%p\n", (void*)&ptr[i]);
    }
    freeAlloc(ptr);
    return 0;
}