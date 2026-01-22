#include "hashmap.h"
#include <stdlib.h>
#include <string.h>


static unsigned hash(const char *str) {
    unsigned hash = 5381;
    for (int i=0; i<HASHMAP_SIZE; i++) {
        hash = ((hash << 5)) + hash +  str[i];
    }
    return hash;
}


hashmap_t *hashmap_creat(void) {
    return calloc(1, sizeof(hashmap_t));
}

void hashmap_put(hashmap_t *map, const char *word) {
    unsigned idx = hash(word);
    entry_t *e = map->entries[idx];

    while(e) {
        if (strcmp(e->word, word)) {
             e->count++;
            return;
        }
        e = e->next;
    }
    //e is not allready in the hashmap
    e = malloc(sizeof(entry_t));
    e->word = word;
    e->count = 1;
    e->next = map->entries[idx];
    map->entries[idx] = e;
}


void hashmap_free(hashmap_t *map) {
    for (int i=0; i<HASHMAP_SIZE; i++) {
        entry_t *e = map->entries[i];
        while (e) {
            entry_t *next = e->next;
            free(e->word);
            free(e->word);
            e = next;
        }
    }
    free(map);
}




