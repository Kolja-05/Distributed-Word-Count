#include "hashmap.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>


static unsigned hash(const char *str) {
    unsigned hash = 5381;
    for (int i=0; i<HASHMAP_SIZE; i++) {
        hash = ((hash << 5)) + hash +  str[i];
    }
    return hash;
}


hashmap_t *hashmap_create(void) {
    return calloc(1, sizeof(hashmap_t));
}

void hashmap_put(hashmap_t *map, const char *word) {
    unsigned idx = hash(word);
    entry_t *e = map->entries[idx];

    while(e) {
        if (strcmp(e->word, word) == 0) {
            size_t len = strlen(e->value);
            e->value = realloc(e->value, len + 2);
            e->value[len] = '1';
            e->value[len+1] = '\0';
            return;
        }
        e = e->next;
    }
    //e is not allready in the hashmap
    e = malloc(sizeof(entry_t));
    e->word = strdup(word);
    e->value = strdup("1");
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

void hashmap_to_string(hashmap_t *map, char *outbuf) {
    size_t pos = 0;
    for (int i=0; i<HASHMAP_SIZE; i++) {
        entry_t *e = map->entries[i];
        while(e) {
            size_t word_len = strlen(e->word);
            size_t value_len = strlen(e->value);
            if (pos + word_len + value_len + 1 < PROTOCOL_MAX_MSG_LEN) {
                memcpy(outbuf + pos, e->word, word_len);
                pos += word_len;
                memcpy(outbuf + pos, e->value, value_len);
                pos += value_len;
                outbuf[pos] = '\0';
            }
            e = e->next;
        }
    }
    outbuf[pos] = '\0';
}


