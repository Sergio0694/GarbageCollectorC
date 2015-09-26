#include "../../Misc/GC_definitions.h"

// Returns the stack pointer
inline void* get_stack_pointer()
{
	asm("movl %esp, %eax");
}

// Returns the program break pointer
inline void* get_heap_pointer()
{
	return sbrk(0);
}