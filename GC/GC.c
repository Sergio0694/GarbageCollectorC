#include "../Misc/GC_definitions.h"
#include "../HashMap/hash_map_t.h"
#include "MemoryHelper/memory_helper.h"

/* =========== Global variables =========== */

bool_t initialized = FALSE;
void* stack_bottom;
hash_map_t allocation_map;

/* ============================================================================
*  Init and allocation functions
*  ========================================================================= */

// Initializes the GarbageCollector in single-thread mode
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

	// Makes sure this function can only be called once
	initialized = TRUE;
}

// Wraps the malloc function
void* GC_alloc(size_t size)
{
	// Calls the standard malloc function to allocate memory
	void* pointer = malloc(size);

	// Stores the reference and the amount of allocated memory
	if (!insert_key(allocation_map, pointer, size))
	{
		ERROR_HELPER("Error inserting a new entry into the hashmap");
	}
	return pointer;
}

// Wraps the realloc function
void* GC_realloc(void* pointer, size_t size)
{
	// Calls the standard realloc function
	void* new_pointer = realloc(pointer, size);

	// Updates the reference in the hash map
	replace_key(allocation_map, pointer, new_pointer);
	return new_pointer;
}

// Wraps the free function
void GC_free(void* pointer)
{
	remove_key(allocation_map, pointer);
}


/* ============================================================================
*  Single thread collect section
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

// Automatically deallocates all the memory blocks that can no longer be reached
void GC_collect()
{
	// Set all the pointers in the allocation map as invalid
	mark_pointers_as_invalid(allocation_map);
	
	// Get the pointer to the top of the stack
	void* address = get_stack_pointer();

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
}