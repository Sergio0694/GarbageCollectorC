#ifndef HASH_H
#define HASH_H

typedef struct hash_map_s* hash_map_t;

hash_map_t hash_map_init();

bool_t insert_key(hash_map_t hm, void* key);
bool_t find_key(hash_map_t hm, void* key);
bool_t remove_key(hash_map_t hm, void* key);
void mark_pointers_as_invalid(hash_map_t hm);
void deallocate_lost_references(hash_map_t hm);
bool_t replace_key(hash_map_t hm, void* old_key, void* new_key);
void mark_as_valid_if_present(hash_map_t hm, void* pointer);
void hash_map_free(hash_map_t hm);

#endif