#include "../Misc/GC_definitions.h"
#include "../HashMap/hash_map_t.h"
#include "MemoryHelper/memory_helper.h"

/* =========== Global variables ===========*/

bool_t initialized = FALSE;
void* heap_end;
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
	heap_end = get_heap_pointer();

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

void GC_collect()
{
	char* program_break = (char*)get_heap_pointer();
	char* stack_top = get_stack_pointer();
	size_t pointer_size = sizeof(void*);

	mark_pointers_as_invalid(allocation_map);

	char* address;
	for (address = heap_end; address <= program_break - pointer_size; address++)
	{
		// Skip the hashmap


		// Heap sweep
		mark_as_valid_if_present(allocation_map, (void*)address);
	}

	for (address = stack_bottom; address >= stack_top - pointer_size; address--)
	{
		mark_as_valid_if_present((void*)address);
	}

	deallocate_lost_references(allocation_map);
}