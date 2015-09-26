#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../Misc/GC_definitions.h"
#include "hash_map_t.h"

/* =========== Local constants ===========*/

#define TRE_QUARTI (3 / 4.0)
#define SENTINEL (void*)-1
#define FIRST_PRIME 257

/* =========== Types used in the file ===========*/

struct pointer_entry_s
{
	void* pointer;
	bool_t valid;
};

typedef pointer_entry_s* pointer_entry_t;

struct hash_map_s
{
	pointer_entry_t* map;
	int current_max_size;
	int current_size;
	int prime_for_hash;
};

/* ===========  PRIVATE FUNCTIONS =========== */
static hash_map_t custom_hash_map_init(size_t size);
static int hash_function_1(int* k, int N);
static int hash_function_2(int* k);
static void rehash(hash_map_t hm);
static int find_position(hash_map_t hm, void* k);
static pointer_entry_s* create_pointer_entry(void* pointer);
/* ========================================== */

hash_map_t hash_map_init()
{
	return custom_hash_map_init(FIRST_PRIME);
}

bool_t insert_key(hash_map_t hm, void* k)
{
	int max_size = hm->current_max_size;
	int i = hash_function_1((int*)k, hm->prime_for_hash);
	if (hm->map[i]->pointer == NULL || hm->map[i]->pointer == SENTINEL)
	{
		free(hm->map[i]);
		hm->map[i] = create_pointer_entry(k);
		hm->current_size += 1;
		if ((hm->current_size / (double)max_size) >= TRE_QUARTI) rehash(hm);
		return TRUE;
	}
	int j;
	int hf2 = hash_function_2((int*)k);
	for (j = 0; j < max_size; ++j)
	{
		int p = (i + j * hf2) % hm->prime_for_hash;
		if (hm->map[p]->pointer == NULL || hm->map[p]->pointer == SENTINEL)
		{
			free(hm->map[p]);
			hm->map[p] = create_pointer_entry(k);
			hm->current_size += 1;
			if ((hm->current_size / (double)max_size) >= TRE_QUARTI) rehash(hm);
			return TRUE;
		}
	}
	return FALSE;
}

void* find_key(hash_map_t hm, void* k)
{
	int max_size = hm->current_max_size;
	int i = hash_function_1((int*)k, hm->prime_for_hash);
	if (hm->map[i]->pointer == k || hm->map[i]->pointer == NULL)) return hm->map[i]->pointer; 
	int j;
	int hf2 = hash_function_2((int*)k);
	for (j = 0; j < max_size; ++j)
	{
		int p = (i + j * hf2) % hm->prime_for_hash;
		if (hm->map[p]->pointer == k || hm->map[p]->pointer == NULL) return hm->map[p]->pointer;
	}
	return NULL;
}

bool_t replace_key(hash_map_t hm, int* old_key, int* new_key)
{
	// TODO
}

bool_t remove_key(hash_map_t hm, void* k)
{
	int i;
	if ((i = find_position(hm, k)) == -1) return FALSE;
	free(hm->map[i]->pointer);
	hm->map[i]->pointer = SENTINEL;
	hm->current_size -= 1;
	return TRUE;
}

void mark_pointers_as_invalid(hash_map_t hm)
{
	int i;
	pointer_entry_t* map = hm->map;
	for (i = 0; i < hm->current_max_size; i++)
	{
		if (map[i]->pointer != NULL && map[i]->pointer != SENTINEL)
		{
			map[i]->valid = FALSE;
		}
	}
}

void mark_as_valid_if_present(hash_map_t hm, void* pointer)
{
	int i = find_position(hm, pointer);
	hm->map[i]->valid = TRUE;
}

void deallocate_lost_references(hash_map_t hm)
{
	int i;
	pointer_entry_t* map = hm->map;
	for (i = 0; i < hm->current_max_sizel; i++)
	{
		if (map[i]->pointer != NULL && map[i]->pointer != SENTINEL)
		{
			if (!map[i]->valid)
			{
				remove_key(hm, map[i]->pointer);
			}
		}
	}
}

void hash_map_free(hash_map_t hm)
{
	int max_size = hm->current_max_size;
	int i;
	for (i = 0; i < max_size; ++i)
	{
		if (hm->map[i] != NULL && (hm->map[i]->pointer == SENTINEL || hm->map[i]->pointer == NULL)) free(hm->map[i]);
		else if (hm->map[i] != NULL && hm->map[i]->pointer != NULL)
		{
			free(hm->map[i]->pointer);
			free(hm->map[i]);
		}
	}
	free(hm->map);
	free(hm);
}

static hash_map_t custom_hash_map_init(size_t size)
{
	hash_map_t to_return = (hash_map_t)malloc(sizeof(struct hash_map_s));
	to_return->map = (pointer_entry_t*)malloc(size * sizeof(pointer_entry_t));

	int i;
	for (i = 0; i < size; ++i) to_return->map[i] = create_pointer_entry(NULL);

	to_return->current_max_size = size;
	to_return->current_size = 0;
	if (is_prime(size)) to_return->prime_for_hash = size;
	else to_return->prime_for_hash = biggest_previous_prime(size);

	return to_return;
}

static pointer_entry_t create_pointer_entry(void* pointer)
{
	pointer_entry_t pointer_entry = (pointer_entry_t)malloc(sizeof(struct pointer_entry_s));
	pointer_entry->pointer = pointer;
	pointer_entry->valid = TRUE;
	return pointer_entry;
}

static int hash_function_1(int* k, int N)
{
	int a = 33, b = 39;
	return (a * (int)k + b) % N;
}

static int hash_function_2(int* k)
{
	int q = 17;
	return q - ((int)k % q);
}

static int find_position(hash_map_t hm, void* k)
{
	int max_size = hm->current_max_size;
	int i = hash_function_1((int*)k, hm->prime_for_hash);
	if (hm->map[i]->pointer == k) return i;
	else if (hm->map[i]->pointer == NULL) return -1;
	int j;
	int hf2 = hash_function_2((int*)k);
	for (j = 0; j < max_size; ++j)
	{
		int p = (i + j * hf2) % hm->prime_for_hash;
		if (hm->map[i]->pointer == k) return i;
		else if (hm->map[i]->pointer == NULL) return -1;
	}
	return -1;
}

static void rehash(hash_map_t hm)
{
	int size = hm->current_max_size;
	hash_map_t hm_new = custom_hash_map_init(2 * size);
	int i;
	for (i = 0; i < size; ++i) if (hm->map[i]->pointer != NULL) insert_key(hm_new, hm->map[i]);
	free(hm->map);
	free(hm);
	hm = custom_hash_map_init(hm_new->current_max_size);
	for (i = 0; i < hm->current_max_size; ++i) if (hm_new->map[i]->pointer != NULL) insert_key(hm, hm_new->map[i]);
	free(hm_new->map);
	free(hm_new);
}

int main()
{
	hash_map_t hm = hash_map_init();
	int* p = (int*)malloc(sizeof(int));
	int* q = (int*)malloc(sizeof(int));
	int* m = (int*)malloc(sizeof(int));
	int* z = (int*)malloc(sizeof(int));
	assert(insert_key(hm, q));
	assert(insert_key(hm, m));
	assert(insert_key(hm, p));

	int i;
	printf("\nCHIAVI:\n");
	for (i = 0; i < hm->current_max_size; i++)
	{
		if (hm->map[i] != NULL) printf("POSITION: %d ---> KEY: %d\n", i, hm->map[i]);
	}

	printf("\nRicerca chiave %d... \n", p);
	printf("Trovata ----> %d\n", find_key(hm, p));

	printf("\nRimozione chiave %d...\n", p);
	assert(remove_key(hm, p));
	printf("Verifica rimozione...\n");
	assert(find_key(hm, p) == NULL);
	printf("Chiave rimossa con successo!\n");

	printf("\nDistruzione hash_map in corso...\n");
	hash_map_free(hm);
	printf("Rimozione hash_map completata\n\n");

	printf("\nProva rehashing...\n");
	hm = custom_hash_map_init(5);
	assert(insert_key(hm, q));
	assert(insert_key(hm, m));
	assert(insert_key(hm, z));
	printf("Rehash?? Current size = %d\n", hm->current_max_size);
	insert_key(hm, p);
	printf("Rehash?? Current size = %d\n", hm->current_max_size);

	free(p);
	free(q);
	free(m);
	free(z);

	printf("%d\n", biggest_previous_prime(1000));
	
	return 0;
}