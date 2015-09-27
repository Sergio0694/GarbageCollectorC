#include "../Misc/GC_definitions.h"
#include "../HashMap/hash_map_t.h"
#include "MemoryHelper/memory_helper.h"

/* =========== Global variables ===========*/

bool_t initialized = FALSE;
//void* heap_end;
void* stack_bottom;
hash_map_t allocation_map;

void GC_init()
{
	if (initialized)
	{
		ERROR_HELPER("The GarbageCollector can't be initialized twice");
	}

	// Calculate initial parameters
	stack_bottom = get_stack_pointer();
	//heap_end = get_heap_pointer();

	// Allocate the hashmap
	allocation_map = hash_map_init();

	initialized = TRUE;
}

void* GC_alloc(size_t size)
{
	void* pointer = malloc(size);
	if (!insert_key(allocation_map, pointer))
	{
		ERROR_HELPER("Error inserting a new entry into the hashmap");
	}
	return pointer;
}

void* GC_realloc(void* pointer, size_t size)
{
	void* new_pointer = realloc(pointer, size);
	replace_key(allocation_map, pointer, new_pointer);
	return new_pointer;
}

/* =========== C program memory structure ===========

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

void recursive_graph_mark(void* pointer, size_t allocated_space)
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

void GC_collect()
{
	// Set all the pointers in the allocation map as invalid
	mark_pointers_as_invalid(allocation_map);
	
	// Stack sweep and mark

	void* address = get_stack_pointer();

	// 4 on 32 bit systems, 8 on 64 bit systems
	void* upper_bound = stack_bottom - sizeof(void*);

	while (address < upper_bound)
	{
		if ((size_t allocated_size = find_key(allocation_map, address)) != 0)
		{
			recursive_graph_mark(address, allocated_size);
		}
		address++;
	}

	deallocate_lost_references(allocation_map);
}