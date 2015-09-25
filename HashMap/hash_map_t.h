#ifndef HASH_H
#define HASH_H

#define SENTINEL (int*)-1

typedef struct hash_map_s* hash_map_t;
typedef enum BOOL_E { FALSE, TRUE } bool_t;

hash_map_t hash_map_init();

bool_t insert_key(hash_map_t hm, int* key);
int* find_key(hash_map_t hm, int* key);
bool_t remove_key(hash_map_t hm, int* key);

void hash_map_free(hash_map_t hm);

#endif