#include "chunking.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>




chunk_list_t chunking_from_file(const char * filepath) {
    chunk_list_t result = {.chunks = NULL, .count = 0};
    FILE * fp = fopen(filepath, "rb");
    if (!fp) {
        return result;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fsize <= 0) {
        fclose(fp);
        return result;
    }
    size_t num_chunks = fsize / MAX_CHUNK_SIZE;
    size_t remaining_bytes = fsize % MAX_CHUNK_SIZE;
    if (remaining_bytes > 0) {
        num_chunks ++;
    }
    result.chunks = malloc(sizeof(chunk_t) * num_chunks);
    if (!result.chunks) {
        fclose(fp);
        return result;
    }
    result.count = num_chunks;
    for (int i=0; i<num_chunks; i++) {
        size_t chunk_size = MAX_CHUNK_SIZE;
        if (i == num_chunks - 1) {
            // last chunk is smaller
            chunk_size = fsize - i * MAX_CHUNK_SIZE;
        }
        result.chunks[i].data = malloc(chunk_size + 1); // +1 for '\0'
        if (!result.chunks[i].data) {
            result.count = i;
            chunking_free(&result);
            fclose(fp);
            return result;
        }
        fread(result.chunks[i].data, 1, chunk_size, fp);
        result.chunks[i].data[chunk_size] = '\0';
        result.chunks[i].length = chunk_size;
    }
    fclose(fp);
    return result;
}




void chunking_free(chunk_list_t *list){
    if (!list) {
        return;
    }
    if (list->chunks) {
        for (int i=0; i<list->count; i++) {
            free(list->chunks[i].data);
        }
        free(list->chunks);
        list->chunks = NULL;
    }
    list->count = 0;
}


