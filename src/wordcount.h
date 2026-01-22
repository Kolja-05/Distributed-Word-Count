#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <stddef.h>
#include <stdbool.h>

#include "hashmap.h"




void wordcount_map(const char *text, hashmap_t *hashmap);

char *wordcount_reduce(const char *text);

#endif




