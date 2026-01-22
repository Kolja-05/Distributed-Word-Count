#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>



//--------HASHMAP-------------

#define HASHMAP_SIZE 1024



typedef struct hash_entry {
    char *word;
    char *value;
    struct hash_entry *next;
} entry_t;

typedef struct hash_map{
    entry_t *entries[HASHMAP_SIZE];
} hashmap_t;

hashmap_t *hashmap_create(void);

void hashmap_put(hashmap_t *map, const char *word);

void hashmap_free(hashmap_t *map);

void hashmap_to_string(hashmap_t *map, char *outbuf);

#endif
