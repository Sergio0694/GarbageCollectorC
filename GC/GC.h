#ifndef GC_H
#define GC_H

#include "..\Misc\GC_definitions"

/* ---------------------------------------------------------------------
*  GC_init
*  ---------------------------------------------------------------------
*  Description:
*    Initializes the GarbageCollector so that it can start checking
*    for memory leaks and fix them
*  Parameters:
*    interval ---> the time in seconds that passes between each memory check.
*        If 0, the GarbageCollector will collect lost references only when
*        explicitly called, otherwise it will run automatically.*/
void GC_init(int interval);

void* GC_alloc(size_t size);

void* GC_realloc(void* pointer, size_t size);

void GC_collect();

#endif