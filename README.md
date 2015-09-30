### GarbageCollectorC

This is a simple implementation of a GarbageCollector in C.
It allows the user to allocate memory using functions that are similar to the standard malloc and realloc functions, without having to worry about lost references and memory leaks.
The GarbageCollector can identify all the memory blocks that can no longer be reached by user code and deallocate them.
While doing so, if the executable is running on a Windows or UNIX system, the GarbageCollector creates a secondary thread and uses that one to perform its operations to avoid blocking the program.

The GarbageCollectorC GC.h file exposes some functions that can be used in every single-thread C program.

```C
/* ---------------------------------------------------------------------
*  GC_init
*  ---------------------------------------------------------------------
*  Description:
*    Initializes the GarbageCollector so that it can start checking
*    for memory leaks and fix them */
void GC_init();

/* ---------------------------------------------------------------------
*  GC_alloc
*  ---------------------------------------------------------------------
*  Description:
*    Wraps the malloc function: allocates a block of memory in the heap
*    and returns a pointer to the first allocated memory location
*  Parameters:
*    size ---> The amount of contiguous space to allocate */
void* GC_alloc(size_t size);

/* ---------------------------------------------------------------------
*  GC_calloc
*  ---------------------------------------------------------------------
*  Description:
*    Wraps the calloc function: allocates a block of memory in the heap,
*    sets all the allocated bytes to 0 and returns a pointer to 
*    the first allocated memory location
*  Parameters:
*    nitems ---> The number of items to request allocated space for
*    size ---> The size of each item */
void* GC_calloc(size_t nitems, size_t size);

/* ---------------------------------------------------------------------
*  GC_realloc
*  ---------------------------------------------------------------------
*  Description:
*    Wraps the realloc function: extends an allocated memory area by
*    copying all the content of the given memory zone to another one and
*    returns a pointer to the new area
*  Parameters:
*    pointer ---> A pointer to the previous allocated space
*    size ---> The size of the new memory block to allocate */
void* GC_realloc(void* pointer, size_t size);

/* ---------------------------------------------------------------------
*  GC_collect
*  ---------------------------------------------------------------------
*  Description:
*    Automatically identifies all the memory blocks in the heap that
*    can no longer be reached by user code and deallocates them */
void GC_collect();

/* ---------------------------------------------------------------------
*  GC_free
*  ---------------------------------------------------------------------
*  Description:
*    Manually frees a block of allocated memory
*  Parameters:
*    pointer ---> The pointer to the first block of the memory area to free */
void GC_free(void* pointer);
```
