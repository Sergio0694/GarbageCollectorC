#ifndef GC_H
#define GC_H

#include "..\Misc\GC_definitions"

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
*    and returns a pointer to the first allocated memory location */
void* GC_alloc(size_t size);

/* ---------------------------------------------------------------------
*  GC_realloc
*  ---------------------------------------------------------------------
*  Description:
*    Wraps the realloc function: extends an allocated memory area by
*    copying all the content of the given memory zone to another one and
*    returns a pointer to the new area */
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
*    Manually frees a block of allocated memory */
void GC_free(void* pointer);

#endif