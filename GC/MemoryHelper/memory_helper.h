#ifndef MEMORY_HELPER_H
#define MEMORY_HELPER_H

/* ---------------------------------------------------------------------
*  get_stack_pointer
*  ---------------------------------------------------------------------
*  Description:
*    Returns the pointer of the current top of the stack */
void* get_stack_pointer();

void* get_start_data_pointer();


void* get_bss_end_pointer();

#endif