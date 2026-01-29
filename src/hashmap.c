#include "hashmap.h"
#include "protocol.h"
#include <stdlib.h>
#include <string.h>


static unsigned hash(const char *str) {
    unsigned hash = 5381;
    while(*str) {
        hash = (hash << 5) + hash + (unsigned char) (*str);
        str++;
    }
    return hash % HASHMAP_SIZE;
}


hashmap_t *hashmap_create(void) {
    return calloc(1, sizeof(hashmap_t));
}

void hashmap_append_one(hashmap_t *map, const char *word) {
    unsigned idx = hash(word);
    entry_t *e = map->entries[idx];

    while(e) {
        if (strcmp(e->word, word) == 0) {
            size_t len = strlen(e->str_value);
            e->str_value = realloc(e->str_value, len + 2);
            e->str_value[len] = '1';
            e->str_value[len+1] = '\0';
            return;
        }
        e = e->next;
    }
    //e is not allready in the hashmap
    e = malloc(sizeof(entry_t));
    e->word = strdup(word);
    e->str_value = strdup("1");
    e->next = map->entries[idx];
    map->entries[idx] = e;
}



void hashmap_add_int_value(hashmap_t *map, const char *word, int value) {
    unsigned idx = hash(word);
    entry_t *e = map->entries[idx];
    while(e) {
        if (strcmp(e->word, word) == 0) {
            e->int_value = e->int_value + value;
            return;
        }
        e = e->next;
    }
    //e is not allready in the hashmap
    e = malloc(sizeof(entry_t));
    e->word = strdup(word);
    e->int_value = value;
    e->next = map->entries[idx];
    map->entries[idx] = e;
}

void hashmap_free(hashmap_t *map) {
    for (int i=0; i<HASHMAP_SIZE; i++) {
        entry_t *e = map->entries[i];
        while (e) {
            entry_t *next = e->next;
            free(e->word);
            if (e->str_value) {
                free(e->str_value);
            }
            free(e);
            e = next;
        }
    }
    free(map);
}

void hashmap_str_values_to_string(hashmap_t *map, char *outbuf) {
    size_t pos = 0;
    for (int i=0; i<HASHMAP_SIZE; i++) {
        entry_t *e = map->entries[i];
        while(e) {
            size_t word_len = strlen(e->word);
            size_t value_len = strlen(e->str_value);
            if (pos + word_len + value_len + 1 < PROTOCOL_MAX_MSG_LEN) {
                memcpy(outbuf + pos, e->word, word_len);
                pos += word_len;
                memcpy(outbuf + pos, e->str_value, value_len);
                pos += value_len;
                outbuf[pos] = '\0';
            }
            e = e->next;
        }
    }
    outbuf[pos] = '\0';
}


void hashmap_int_values_to_string(hashmap_t *map, char *outbuf) {

}
