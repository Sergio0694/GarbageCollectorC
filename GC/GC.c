// Default libraries
#include "../Misc/GC_definitions.h"
#include "../HashMap/hash_map_t.h"
#include "MemoryHelper/memory_helper.h"
#include "SharedCode\GC_shared.h"

// On Unix-like OSes, switch to a multithread GC
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#include <semaphore.h>
#include <pthread.h>
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
sem_t* mutual_exclusion_semaphore;
#elif defined WIN_THREADS
HANDLE shared_mutex;

// Tries to access the critical section on Windows
static inline void try_get_mutex(HANDLE mutex)
{
	DWORD result = WaitForSingleObject(shared_mutex, INFINITE);
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

	// Semaphores initialization
#if defined POSIX_THREADS
	if (sem_init(mutual_exclusion_semaphore, 0, 1) == -1)
#elif defined WIN_THREADS
	if (CreateMutex(NULL, FALSE, NULL) == NULL)
#endif
#if defined POSIX_THREADS || defined WIN_THREADS
	{
		ERROR_HELPER("Error creating the semaphore");
	};
#endif

	// Makes sure this function can only be called once
	initialized = TRUE;
}

// Wraps the malloc function
void* GC_alloc(size_t size)
{
#if defined POSIX_THREADS
	sem_wait(mutual_exclusion_semaphore);
#elif defined WIN_THREADS
	try_get_mutex(shared_muted);
#endif

	// Calls the standard malloc function to allocate memory
	void* pointer = malloc(size);

	// Stores the reference and the amount of allocated memory
	if (!insert_key(allocation_map, pointer, size))
	{
		ERROR_HELPER("Error inserting a new entry into the hashmap");
	}

#if defined POSIX_THREADS
	sem_post(mutual_exclusion_semaphore);
#else
	try_release_mutex(shared_muted);
#endif
	return pointer;
}

// Wraps the realloc function
void* GC_realloc(void* pointer, size_t size)
{
#if defined POSIX_THREADS
	sem_wait(mutual_exclusion_semaphore);
#elif defined WIN_THREADS
	try_get_mutex(shared_muted);
#endif

	// Calls the standard realloc function
	void* new_pointer = realloc(pointer, size);

	// Updates the reference in the hash map
	replace_key(allocation_map, pointer, new_pointer);

#if defined POSIX_THREADS
	sem_post(mutual_exclusion_semaphore);
#else
	try_release_mutex(shared_muted);
#endif
	return new_pointer;
}

// Wraps the free function
void GC_free(void* pointer)
{
#if defined POSIX_THREADS
	sem_wait(mutual_exclusion_semaphore);
#elif defined WIN_THREADS
	try_get_mutex(shared_muted);
#endif
	remove_key(allocation_map, pointer);
#if defined POSIX_THREADS
	sem_post(mutual_exclusion_semaphore);
#else
	try_release_mutex(shared_muted);
#endif
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
		// Check if the current location is a pointer to an dynamically allocated memory block
		size_t allocated_size = find_key(allocation_map, address)
			if (allocated_size != 0)
			{
				// Use that block as the root and mark all the memory graph as reachable
				recursive_graph_mark(address, allocated_size);
			}
		address++;
	}

	// Deallocate all the references that are definitively lost
	deallocate_lost_references(allocation_map);

#if defined POSIX_THREADS
	sem_post(mutual_exclusion_semaphore);
#else
	try_release_mutex(shared_muted);
#endif
}

// Automatically deallocates all the memory blocks that can no longer be reached
void GC_collect()
{
	// Get the pointer to the top of the stack
	void* address = get_stack_pointer();

#if defined POSIX_THREADS
	sem_wait(mutual_exclusion_semaphore);

	// Thread initialization and call to GC_main
	pthread_t* gc_main_thread = malloc(sizeof(pthread_t));
	if (pthread_create(gc_main_thread, NULL, GC_main, address) != 0)
	{
		ERROR_HELPER("Error creating the thread");
	}
#elif defined WIN_THREADS
	try_get_mutex(shared_muted);

	HANDLE gc_main_thread = CreateThread(NULL, 0, collect, address, 0, NULL);
	if (gc_main_thread == NULL)
	{
		ERROR_HELPER("Error creating the thread");
	}
#else
	collect(address);
#endif
}