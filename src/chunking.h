#ifndef CHUNKING_H
#define CHUNKING_H

#include <stddef.h>
#include "protocol.h"


#define MAX_CHUNK_SIZE PROTOCOL_MAX_MSG_LEN - PROTOCOL_TYPE_LEN

typedef struct chunk {
    char * data;
    size_t length;
} chunk_t;


typedef struct chunk_list {
    chunk_t * chunks;
    size_t count;
} chunk_list_t;


chunk_list_t chunking_from_file(const char * filepath);

void chunking_free(chunk_list_t *chunks);

#endif
