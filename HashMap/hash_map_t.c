#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../Misc/GC_definitions.h"
#include "hash_map_t.h"

/* =========== Local constants ===========*/

#define CAPACITY_THRESHOLD 74
#define SENTINEL (void*)-1
#define FIRST_PRIME 257
#define VALID_THRESHOLD 1

/* =========== Types used in the file ===========*/

// Struct that holds the allocated memory block address, its size and a flag
struct pointer_entry_s
{
	void* pointer;
	size_t size;
	bool_t valid;
};

// The type used in the hash map functions
typedef pointer_entry_s* pointer_entry_t;

/* ---------------------------------------------------------------------
*  hash_map_s
*  ---------------------------------------------------------------------
*  Description:
*    The struct that contains all the info for a given hash map instance
*  Fields:
*    map ---> The hash map of pointer_entry_t, holds all the used references 
*    current_max_size ---> The size of the hash map
*    current_size ---> The actual number of items in the hash map
*    prime_for_hash ---> Prime number used to perform the hashing operations */
struct hash_map_s
{
	pointer_entry_t* map;
	int current_max_size;
	int current_size;
	int prime_for_hash;
};

/* ============================================================================
*  Map creation functions
*  ========================================================================= */

/* ---------------------------------------------------------------------
*  custom_hash_map_init
*  ---------------------------------------------------------------------
*  Description:
*    Creates a new hash map with the desired size
*  Parameters:
*    size ---> The maximum value for the size of the new hash map */
static hash_map_t custom_hash_map_init(size_t size)
{
	// Allocate the struct and the array used as the hash map
	hash_map_t to_return = (hash_map_t)malloc(sizeof(struct hash_map_s));
	to_return->map = (pointer_entry_t*)malloc(size * sizeof(pointer_entry_t));

	// Initialize all the array items to NULL
	int i;
	for (i = 0; i < size; ++i)
	{
		to_return->map[i] = NULL;
	}

	// Set the default hash map parameters
	to_return->current_max_size = size;
	to_return->current_size = 0;

	// Calculate the new prime number to use in the hash map
	if (is_prime(size)) to_return->prime_for_hash = size;
	else
	{
		to_return->prime_for_hash = biggest_previous_prime(size);
	}
	return to_return;
}

// Creates and returns an empty hash map
hash_map_t hash_map_init()
{
	return custom_hash_map_init(FIRST_PRIME);
}

/* ============================================================================
*  Hash functions
*  ========================================================================= */

// Private functions prototypes
static int find_position(hash_map_t hm, void* k);
static bool_t insert_entry(hash_map_t hm, pointer_entry_t pe)

/* ---------------------------------------------------------------------
*  rehash
*  ---------------------------------------------------------------------
*  Description:
*    Creates a new hash map than can contain twice as many items as the
*    previous one, moves the content of the first map into the second
*    one and then deallocates the first hash map
*  Parameters:
*    hm ---> The hash map to destroy */
static void rehash(hash_map_t hm)
{
	int size = hm->current_max_size;
	hash_map_t hm_new = custom_hash_map_init(2 * size);
	int i;
	for (i = 0; i < size; ++i)
	{
		if (hm->map[i] != NULL && hm->map[i] != SENTINEL) insert_entry(hm_new, hm->map[i]);
	}
	free(hm->map);
	free(hm);
	hm = custom_hash_map_init(hm_new->current_max_size);
	for (i = 0; i < hm->current_max_size; ++i)
	{
		if (hm_new->map[i] != NULL && hm->map[i] != SENTINEL) insert_entry(hm, hm_new->map[i]);
	}
	free(hm_new->map);
	free(hm_new);
}

/* ---------------------------------------------------------------------
*  hash_function_1
*  ---------------------------------------------------------------------
*  Description:
*    Main hash function with Horner's method
*  Parameters:
*    k ---> The value to hash
*    n ---> Upper bound for the hash result */
static int hash_function_1(int* k, int n)
{
	const int a = 33;
	int len;
	int* digits = extract_digits_array((int)k, &len);
	int sum = digits[len - 1], i;
	for (i = len - 2; >= 0; i--)
	{
		sum *= a;
		sum += digits[i];
	}
	return abs(sum % n);
}

/* ---------------------------------------------------------------------
*  hash_function_2
*  ---------------------------------------------------------------------
*  Description:
*    Secondary hash function, used whenever a collision occurs
*  Parameters:
*    k ---> The value to hash */
static int hash_function_2(int* k)
{
	int q = 17;
	return q - ((int)k % q);
}

/* ============================================================================
*  Internal hash map functions
*  ========================================================================= */

static int find_position(hash_map_t hm, void* k)
{
	int max_size = hm->current_max_size;
	int i = hash_function_1((int*)k, hm->prime_for_hash);
	if (hm->map[i] != SENTINEL && hm->map[i] != NULL && hm->map[i]->pointer == k) return i;
	else if (hm->map[i] == NULL) return -1;
	int j;
	int hf2 = hash_function_2((int*)k);
	for (j = 0; j < max_size; ++j)
	{
		int p = (i + j * hf2) % hm->prime_for_hash;
		if (hm->map[p] != SENTINEL && hm->map[p] != NULL && hm->map[p]->pointer == k) return i;
		else if (hm->map[p] == NULL) return -1;
	}
	return -1;
}

static bool_t insert_entry(hash_map_t hm, pointer_entry_t pe)
{
	void* key = pe->pointer;
	size_t size = pe->size;
	bool_t success = insert_key(hm, key, size);
	if (success)
	{
		int i = find_position(hm, key);
		hm->map[i]->valid = pe->valid;
	}
	return success;
}

static pointer_entry_t create_pointer_entry(void* pointer, size_t size)
{
	pointer_entry_t pointer_entry = (pointer_entry_t)malloc(sizeof(struct pointer_entry_s));
	pointer_entry->pointer = pointer;
	pointer_entry->size = size;
	return pointer_entry;
}

/* ============================================================================
*  Public hash map functions
*  ========================================================================= */

// Inserts a new key into the target hash map
bool_t insert_key(hash_map_t hm, void* k, size_t size)
{
	int max_size = hm->current_max_size;
	int i = hash_function_1((int*)k, hm->prime_for_hash);
	if (hm->map[i] == NULL || hm->map[i] == SENTINEL)
	{
		hm->map[i] = create_pointer_entry(k, size);
		hm->current_size += 1;
		if (100 * hm->current_size / max_size) >= CAPACITY_THRESHOLD) rehash(hm);
		return TRUE;
	}
	int j;
	int hf2 = hash_function_2((int*)k);
	for (j = 0; j < max_size; ++j)
	{
		int p = (i + j * hf2) % hm->prime_for_hash;
		if (hm->map[p] == NULL || hm->map[p] == SENTINEL)
		{
			hm->map[p] = create_pointer_entry(k, size);
			hm->current_size += 1;
			if (100 * hm->current_size / max_size) >= CAPACITY_THRESHOLD) rehash(hm);
			return TRUE;
		}
	}
	return FALSE;
}

// Checks if the given key exists in the target hash map
size_t find_key(hash_map_t hm, void* k)
{
	int max_size = hm->current_max_size;
	int i = hash_function_1((int*)k, hm->prime_for_hash);
	void* temp = hm->map[i]->pointer;
	if (temp == k) return hm->map[i]->size;
	else if (temp == NULL) return 0;
	int j;
	int hf2 = hash_function_2((int*)k);
	for (j = 0; j < max_size; ++j)
	{
		int p = (i + j * hf2) % hm->prime_for_hash;
		temp = hm->map[p]->pointer;
		if (temp == k) return hm->map[p]->size;
		else if (temp == NULL) return 0;
	}
	return 0;
}

// Remove a given key from the hash map
bool_t remove_key(hash_map_t hm, void* k)
{
	int i;
	if ((i = find_position(hm, k)) == -1) return FALSE;
	free(hm->map[i]->pointer);
	free(hm->map[i]);
	hm->map[i] = SENTINEL;
	hm->current_size -= 1;
	return TRUE;
}

// Removes the first key and inserts the new one
bool_t replace_key(hash_map_t hm, void* old_key, void* new_key)
{
	if (!remove_key(hm, old_key)) return FALSE;
	return insert_key(hm, new_key);
}

// Deallocates the target hash map
void hash_map_free(hash_map_t hm)
{
	int max_size = hm->current_max_size;
	int i;
	for (i = 0; i < max_size; ++i)
	{
		if (hm->map[i] != NULL && hm->map[i] != SENTINEL)
		{
			free(hm->map[i]->pointer);
			free(hm->map[i]);
		}
	}
	free(hm->map);
	free(hm);
}

/* ============================================================================
*  GC utility functions
*  ========================================================================= */

// Sets the "valid" parameter of all the items into the hash map to FALSE
void mark_pointers_as_invalid(hash_map_t hm)
{
	int i;
	pointer_entry_t* map = hm->map;
	for (i = 0; i < hm->current_max_size; i++)
	{
		if (map[i] != NULL && map[i] != SENTINEL)
		{
			map[i]->valid = FALSE;
		}
	}
}

// Mark the given key as valid if it is present inside the hash map
void mark_as_valid_if_present(hash_map_t hm, void* pointer)
{
	int i = find_position(hm, pointer);
	if (i == -1) return;
	hm->map[i]->valid = TRUE;
}

// Deallocates and removes all the invalid items inside the hash map
void deallocate_lost_references(hash_map_t hm)
{
	int i;
	pointer_entry_t* map = hm->map;
	for (i = 0; i < hm->current_max_sizel; i++)
	{
		if (map[i] != NULL && map[i] != SENTINEL)
		{
			if (!map[i]->valid)
			{
				remove_key(hm, map[i]->pointer);
			}
		}
	}
}

int main()
{
	
	return 0;
}