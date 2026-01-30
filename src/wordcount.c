#include "wordcount.h"
#include "hashmap.h"
#include "protocol.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void wordcount_map(const char *text, hashmap_t *hashmap) {
    char word[MAX_WORD_LEN] = {0}; // initialize to avoid unknown behaviour
    int pos = 0;
    for (int i=0; text[i]!= '\0'; i++) {
        if (isalpha((unsigned char) text[i])) {
            if (pos < MAX_WORD_LEN - 1) {
                word[pos] = tolower((unsigned char) text[i]);
                pos++;
            }
        }
        else {
            if (pos > 0) {
                word[pos] = '\0';
                hashmap_append_one(hashmap, word);
                pos = 0;
            }
        }
    }
    if (pos > 0) {
        word[pos] = '\0';
        hashmap_append_one(hashmap, word);
    }
}

void wordcount_ones_string_to_number(const char *input, char *output) {
    int input_idx = 0;
    int output_idx = 0;
    while (input[input_idx]) {
        if (isdigit(input[input_idx])) {
            int sum = 0;
            while (isdigit(input[input_idx])) {
                sum++;
                input_idx++;
            }
            output_idx += snprintf(&output[output_idx], PROTOCOL_MAX_MSG_LEN - output_idx, "%d", sum);
        }
        else {
            output[output_idx++] = input[input_idx++];
        }
    }
    output[output_idx] = '\0';
}

void wordcount_reduce(const char *input, hashmap_t *hashmap) {
    char word[MAX_WORD_LEN] = {0}; // initialize to avoid unknown behaviour
    int pos = 0;
    for (int i=0; input[i]!= '\0'; i++) {
        if (isalpha(input[i])) {
            if (pos < MAX_WORD_LEN - 1) {
                word[pos] = tolower(input[i]);
                pos++;
            }
        }
        else {
            if (pos > 0) {
                word[pos] = '\0';
                int count = atoi(&input[i]);
                hashmap_add_int_value(hashmap, word, count);
                pos = 0;
            }
        }
    }
}
int wordcount_hashmap_to_array(hashmap_t *map, wordcount_pair_t **out) {
    // count entries
    int out_len = 0;
    for (int i=0; i<HASHMAP_SIZE; i++) {
        entry_t *e = map->entries[i];
        while(e) {
            out_len++;
            e = e->next;
        }
    }
    *out = calloc(out_len, sizeof(wordcount_pair_t));
    if (!*out) {
        fprintf(stderr, "calloc failed\n");
        return -1;
    }
    int idx = 0;
    for (int i=0; i<HASHMAP_SIZE; i++) {
        entry_t *e = map->entries[i];
        while(e) {
            (*out + idx)->word = e->word;
            (*out + idx)->count = e->int_value;
            e = e->next;
            idx++;
        }
    }
    return out_len;
}

int wordcount_compare(const void *a, const void *b) {
    wordcount_pair_t *word_a = (wordcount_pair_t*) a;
    wordcount_pair_t *word_b = (wordcount_pair_t*) b;

    if (word_a->count != word_b->count) {
        // counts are not eaqual -> order is decided by the count
        return word_b->count - word_a->count;
    }
    else {
        // equal count -> lexicographic order should decide
        return strcmp(word_a->word, word_b->word); // if words are equal, we messed up combine
    }
}

void wordcount_print(wordcount_pair_t *result, size_t len) {
    printf("word,frequency\n");
    for (int i=0; i<len; i++) {
        printf("%s,%d\n", result[i].word, result[i].count);
    }
}
