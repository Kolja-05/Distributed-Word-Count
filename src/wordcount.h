#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <stddef.h>
#include <stdbool.h>



char *wordcount_map(const char *text);

char *wordcount_reduce(const char *text);

#endif




