#ifndef MEMORY_HELPER_H
#define MEMORY_HELPER_H

/* ---------------------------------------------------------------------
*  get_stack_pointer
*  ---------------------------------------------------------------------
*  Description:
*    Returns the pointer of the current top of the stack */
inline void* get_stack_pointer();

/* ---------------------------------------------------------------------
*  get_heap_pointer
*  ---------------------------------------------------------------------
*  Description:
*    Returns the pointer to the top of the heap */
inline void* get_heap_pointer();

#endif