# First fit allocator

<sub>Based on Marwan Burelle's 2009 malloc tutorial, adapted to use `mmap` instead of `sbrk`.</sub>

This is a very simple first fit allocator using `mmap`.<br>
It has:

* `alloc()`
* `free()`
* `realloc()`
* `reallocf()`
* `calloc()`

I've tried to document everything I've done in a not very redundant way.<br>
There is an example in `src/main.c`

Currently known limitations:

* `fusion()` only checks for free succesor block but it could check for free predecessor blocks