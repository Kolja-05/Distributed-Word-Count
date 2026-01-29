#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <stddef.h>
#include <stdbool.h>

#include "hashmap.h"



#define MAX_WORD_LEN 64

void wordcount_map(const char *text, hashmap_t *hashmap);

void wordcount_ones_string_to_number(const char *input, char *output);

void wordcount_reduce(const char *input, hashmap_t *hashmap);

#endif




