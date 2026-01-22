#include "wordcount.h"
#include "hashmap.h"
#include <ctype.h>
#include <string.h>


#define MAX_WORD_LEN 32

void wordcount_map(const char *text, hashmap_t *hashmap) {
    char word[MAX_WORD_LEN];
    int pos = 0;
    for (int i=0; text[i]!= '\0'; i++) {
        if (isalpha(text[i])) {
            if (pos < MAX_WORD_LEN - 1) {
                word[pos] = tolower(text[i]);
                pos++;
            }
        }
        else {
            if (pos > 0) {
                word[pos] = '\0';
                hashmap_put(hashmap, word);
                pos = 0;
            }
        }
    }
    if (pos > 0) {
        word[pos] = '\0';
        hashmap_put(hashmap, word);
    }
}
