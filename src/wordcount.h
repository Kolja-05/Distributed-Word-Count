#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <stddef.h>
#include <stdbool.h>

#include "hashmap.h"




void wordcount_map(const char *text, hashmap_t *hashmap);

void wordcount_ones_string_to_number(char *input, char *output);

#endif




