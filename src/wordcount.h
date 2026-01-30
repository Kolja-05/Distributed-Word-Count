#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "hashmap.h"


typedef struct {
    char *word;
    int count;
} wordcount_pair_t;


#define MAX_WORD_LEN 64

void wordcount_map(const char *text, hashmap_t *hashmap);

void wordcount_ones_string_to_number(const char *input, char *output);

void wordcount_reduce(const char *input, hashmap_t *hashmap);

int wordcount_hashmap_to_array(hashmap_t *map, wordcount_pair_t **out);

int wordcount_compare(const void * a, const void * b);

void wordcount_print(wordcount_pair_t *result, size_t len);

 #endif



