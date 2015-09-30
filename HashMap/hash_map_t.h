#ifndef HASH_H
#define HASH_H

// The hash map used by the GC
typedef struct hash_map_s* hash_map_t;

/* ============================================================================
*  Generic hash map functions
*  ========================================================================= */

/* ---------------------------------------------------------------------
*  hash_map_init
*  ---------------------------------------------------------------------
*  Description:
*    Allocates and initializes a new hash map ready to use */
hash_map_t hash_map_init();

/* ---------------------------------------------------------------------
*  insert_key
*  ---------------------------------------------------------------------
*  Description:
*    Inserts a new item into the hash map, performing the rehash
*    automatically if necessary
*  Parameters:
*    hm ---> The hash map currently in use
*    key ---> The new pointer to insert into the hash map
*    size ---> The size of the allocated area referenced by the pointer */
bool_t insert_key(hash_map_t hm, void* key, size_t size);

/* ---------------------------------------------------------------------
*  find_key
*  ---------------------------------------------------------------------
*  Description:
*    Checks if a given address is present into the hash map.
*    It returns TRUE if present, FALSE otherwise
*  Parameters:
*    hm ---> The hash map currently in use
*    key ---> The pointer to find inside the hash map */
bool_t find_key(hash_map_t hm, void* key);

/* ---------------------------------------------------------------------
*  remove_key
*  ---------------------------------------------------------------------
*  Description:
*    Removes a given address from the hash map. Returns TRUE if the
*    operation succedes, FALSE if the key isn't found inside the hash map
*  Parameters:
*    hm ---> The hash map currently in use
*    key ---> The pointer to find and remove from the hash map */
bool_t remove_key(hash_map_t hm, void* key);

/* ---------------------------------------------------------------------
*  replace_key
*  ---------------------------------------------------------------------
*  Description:
*    Removes a given address from the hash map and replaces it with
*    another one. Returns TRUE if the operation succedes (if the first
*    key is found inside the hash map and removed correctly), 
*    FALSE if the key isn't found inside the hash map
*  Parameters:
*    hm ---> The hash map currently in use
*    old_key ---> The pointer to find and remove from the hash map
*    new_key ---> The new value to insert into the hash map */
bool_t replace_key(hash_map_t hm, void* old_key, void* new_key);

/* ---------------------------------------------------------------------
*  hash_map_free
*  ---------------------------------------------------------------------
*  Description:
*    Deallocates all the memory areas that are referenced by pointers stored
*    inside the hash map (so all the memory areas that were previously
*    allocated through the GC), then deallocates the hash map itself
*  Parameters:
*    hm ---> The hash map to deallocate */
void hash_map_free(hash_map_t hm);

/* ============================================================================
*  GC utility functions
*  ========================================================================= */

/* ---------------------------------------------------------------------
*  mark_pointers_as_invalid
*  ---------------------------------------------------------------------
*  Description:
*    Marks all the references in the hash map as invalid. This function
*    is used by the GC before starting the collect operation
*  Parameters:
*    hm ---> The hash map in use */
void mark_pointers_as_invalid(hash_map_t hm);

/* ---------------------------------------------------------------------
*  mark_as_valid_if_present
*  ---------------------------------------------------------------------
*  Description:
*    Checks if a given pointer is present, and marks it as valid if 
*    it is found inside the hash map
*  Parameters:
*    hm ---> The hash map in use
*    pointer ---> The pointer to look for inside the hash map */
void mark_as_valid_if_present(hash_map_t hm, void* pointer);

/* ---------------------------------------------------------------------
*  deallocate_lost_references
*  ---------------------------------------------------------------------
*  Description:
*    Deallocates all the memory areas that are referenced by pointers
*    inside the hash map that are marked as invalid
*  Parameters:
*    hm ---> The hash map in use */
void deallocate_lost_references(hash_map_t hm);

#endif