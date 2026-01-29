#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>



//--------HASHMAP-------------

#define HASHMAP_SIZE 512*512


typedef struct hash_entry {
    char *word;
    char *str_value;
    int int_value;
    struct hash_entry *next;
} entry_t;

typedef struct hash_map{
    entry_t *entries[HASHMAP_SIZE];
} hashmap_t;

hashmap_t *hashmap_create(void);

void hashmap_append_one(hashmap_t *map, const char *word);

void hashmap_add_int_value(hashmap_t *map, const char *word, int value);

void hashmap_free(hashmap_t *map);

void hashmap_str_values_to_string(hashmap_t *map, char *outbuf);

void hashmap_int_values_to_string(hashmap_t *map, char *outbuf);

#endif
