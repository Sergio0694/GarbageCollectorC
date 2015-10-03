// Default libraries
#include "../Misc/GC_definitions.h"
#include "../HashMap/hash_map_t.h"
#include "MemoryHelper/memory_helper.h"
#include "SharedCode\GC_shared.h"

// On Unix-like OSes, switch to a multithread GC
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <pthread.h>
#include<unistd.h>
#define POSIX_THREADS
#elif defined _WIN32
#include <windows.h>
#define WIN_THREADS
#endif

/* =========== Global variables and local functions =========== */

// Global shared variables
bool_t initialized = FALSE;
void* stack_bottom;
hash_map_t allocation_map;

// OS-specific global variables
#if defined POSIX_THREADS

// UNIX global mutex and macros
pthread_mutex_t shared_lock;
#define GET_LOCK pthread_mutex_lock(&shared_lock);
#define RELEASE_LOCK pthread_muted_unlock(&shared_lock);
#elif defined WIN_THREADS

// WIN32 global mutex and helper functions
HANDLE shared_lock;

// Tries to access the critical section on Windows
static inline void try_get_mutex(HANDLE mutex)
{
	DWORD result = WaitForSingleObject(shared_lock, INFINITE);
	if (result != WAIT_OBJECT_0)
	{
		ERROR_HELPER("Error getting the mutex");
	}
}

// Tries to release the mutex and to exit from the critical section
static inline void try_release_mutex(HANDLE mutex)
{
	if (!ReleaseMutex(mutex))
	{
		ERROR_HELPER("Error releasing the mutex");
	}
}

// WIN32 macros
#define GET_LOCK try_get_mutex(shared_lock)
#define RELEASE_LOCK try_release_mutex(shared_lock)
#endif

/* ============================================================================
*  Init and allocation functions
*  ========================================================================= */

// Initializes the GarbageCollector
void GC_init()
{
	// Error check
	if (initialized)
	{
		ERROR_HELPER("The GarbageCollector can't be initialized twice");
	}

	// Saves the stack pointer, it will be used during the sweep process
	stack_bottom = get_stack_pointer();

	// Allocate the hashmap to hold the references to the allocated memory
	allocation_map = hash_map_init();

	// Mutex initialization
#if defined POSIX_THREADS
	if (pthread_mutex_init(&shared_lock, NULL) != 0)
#elif defined WIN_THREADS
	if (CreateMutex(NULL, FALSE, NULL) == NULL)
#endif
#if defined POSIX_THREADS || defined WIN_THREADS
	{
		ERROR_HELPER("Error creating the mutex");
	};
#endif

	// Makes sure this function can only be called once
	initialized = TRUE;
}

// Wraps the malloc function
void* GC_alloc(size_t size)
{
	GET_LOCK;

	// Calls the standard malloc function to allocate memory
	void* pointer = malloc(size);

	// Stores the reference and the amount of allocated memory
	if (!insert_key(allocation_map, pointer, size))
	{
		ERROR_HELPER("Error inserting a new entry into the hashmap");
	}

	RELEASE_LOCK;
	return pointer;
}

// Wraps the calloc function
void* GC_calloc(size_t nitems, size_t size)
{
	GET_LOCK;

	// Calls the standard calloc function to allocate memory
	void* pointer = calloc(nitems, size);

	// Stores the reference and the amount of allocated memory
	if (!insert_key(allocation_map, pointer, nitems * size))
	{
		ERROR_HELPER("Error inserting a new entry into the hashmap");
	}

	RELEASE_LOCK;
	return pointer;
}

// Wraps the realloc function
void* GC_realloc(void* pointer, size_t size)
{
	GET_LOCK;

	// Calls the standard realloc function
	void* new_pointer = realloc(pointer, size);

	// Updates the reference in the hash map
	replace_key(allocation_map, pointer, new_pointer);

	RELEASE_LOCK;
	return new_pointer;
}

// Wraps the free function
void GC_free(void* pointer)
{
	GET_LOCK;
	remove_key(allocation_map, pointer);
	RELEASE_LOCK;
}

/* ============================================================================
*  Collect section
*  ============================================================================

>> C program memory structure:

╔══ Higher addresses
║
╠══ STACK ═════════
║     |
║     v
║
║     ^
║     |
╠══ HEAP ══════════
║
╚══ Lower addresses

========================================== */

// Explores a graph of allocated memory nodes and marks all of them as valid
static void recursive_graph_mark(void* pointer, size_t allocated_space)
{
	void* upper_bound = pointer + allocated_space - sizeof(void*);
	while (pointer < upper_bound)
	{
		size_t allocated_size = find_key(allocation_map, address);
		if ((allocated_size != 0) && !check_if_marked(allocation_map, pointer))
		{
			mark_as_valid_if_present(allocation_map, pointer);
			recursive_graph_mark(pointer, allocated_size);
		}
		pointer++;
	}
}

// Main function for the collect operation
#if WIN_THREADS
DWORD WINAPI collect(LPVOID lparam)
{
	// Explicit argument cast
	void* address = (void*)lparam;
#else
static void collect(void* address)
#endif
{
	// Set all the pointers in the allocation map as invalid
	mark_pointers_as_invalid(allocation_map);

	// Calculate the upper address, sizeof(void*) is 4 on 32 bit systems and 8 on 64 bit systems
	void* upper_bound = stack_bottom - sizeof(void*);

	// Sweep the stack
	while (address < upper_bound)
	{
		// Check if the current location is a pointer to an dynamically allocated memory bshared_lock
		size_t allocated_size = find_key(allocation_map, address)
			if (allocated_size != 0)
			{
				// Use that bshared_lock as the root and mark all the memory graph as reachable
				recursive_graph_mark(address, allocated_size);
			}
		address++;
	}

	// Deallocate all the references that are definitively lost
	deallocate_lost_references(allocation_map);

	RELEASE_LOCK;
}

// Backups the content of the general purpose registers into a target array
static void populate_registers_array(void* target[])
{
// The buffer will contain "movl/q %0xADDRESS, %xxx"
#define COMMAND_BUFFER_LENGTH = 64

// Enumerate the fourteen 64 bit registers
#if defined(__x86_64__) || defined(_M_X64)
#define REG_NUMBER 16
#define MOV_COMMAND "movq"
	char* registers[] = { "rax", "rbx", "rcx", "rdx", "rdi", "rsi", 
						"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };
#else

// 32 bit systems only have 6 general purpose registers
#define REG_NUMBER 8
#define MOV_COMMAND "movl"
	char* registers[] = { "eax", "ebx", "ecx", "edx", "edi", "esi" };
	void* registers[8];
#endif

	// Run the asm commands to copy the registers content into the array
	int i;
	for (i = 0; i < REG_NUMBER; i++)
	{
		char command[COMMAND_BUFFER_LENGTH];
		sprintf(command, "%s %s, \%%p", MOV_COMMAND, registers[i], target + i);
		asm(command);
	}
}

// Automatically deallocates all the memory bshared_locks that can no longer be reached
void GC_collect()
{
	// Backup the content of the general purpose registers into the stack
#if defined(__x86_64__) || defined(_M_X64)
	void* registers_backup[16];
#else
	void* registers_backup[8];
#endif
	populate_registers_array(registers_backup);

	// Get the pointer to the top of the stack
	void* address = get_stack_pointer();

#if defined POSIX_THREADS
	GET_LOCK;

	// Thread initialization and call to GC_main
	pthread_t* gc_main_thread = malloc(sizeof(pthread_t));
	if (pthread_create(gc_main_thread, NULL, GC_main, address) != 0)
	{
		ERROR_HELPER("Error creating the thread");
	}
#elif defined WIN_THREADS
	GET_LOCK;

	HANDLE gc_main_thread = CreateThread(NULL, 0, collect, address, 0, NULL);
	if (gc_main_thread == NULL)
	{
		ERROR_HELPER("Error creating the thread");
	}
#else
	collect(address);
#endif
}