#include "wordcount.h"
#include "hashmap.h"
#include "protocol.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


#define MAX_WORD_LEN 32

void wordcount_map(const char *text, hashmap_t *hashmap) {
    char word[MAX_WORD_LEN] = {0}; // initialize to avoid unknown behaviour
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

void wordcount_ones_String_to_number(char *input, char *output) {
    int input_idx = 0;
    int output_idx = 0;
    while (input[input_idx]) {
        if (isdigit(input[input_idx])) {
            int sum = 0;
            while (isdigit(input[input_idx])) {
                sum++;
                input_idx++;
            }
            output_idx = snprintf(&output[output_idx], PROTOCOL_MAX_MSG_LEN, "%d", sum);
        }
        else {
            output[output_idx++] = input[input_idx++];
        }
    }
    output[output_idx] = '\0';
}


