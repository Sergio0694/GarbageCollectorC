#include "../../Misc/GC_definitions.h"

// Returns the stack pointer
inline void* get_stack_pointer()
{
	// Checks the current CPU architecture
#if defined(__x86_64__) || defined(_M_X64)
	asm("movl %rsp, %rax");
#else
	asm("movl %esp, %eax");
#endif
}

// Returns the program break pointer
inline void* get_heap_pointer()
{
	return sbrk(0);
}