#include "../../Misc/GC_definitions.h"

// Returns the stack pointer
inline void* get_stack_pointer()
{
	// Checks the current CPU architecture
#if defined(__x86_64__) || defined(_M_X64)
	asm("movq %rsp, %rax");
#else
	asm("movl %esp, %eax");
#endif
}

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
extern char edata;

void* get_start_data_pointer()
{
	return &edata;
}


void* get_bss_end_pointer()
{
	//return sbrk(0);
}