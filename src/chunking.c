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
    long fsize_long = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fsize_long < 0) {
        fclose(fp);
        return result;
    }
    size_t fsize = (size_t) fsize_long;
    size_t max_chunk = (size_t) MAX_CHUNK_SIZE;
    size_t num_chunks = fsize / max_chunk;
    size_t remaining_bytes = fsize % (size_t) max_chunk;

    if (remaining_bytes > 0) {
        num_chunks ++;
    }


   result.chunks = malloc(sizeof(chunk_t) * num_chunks);
    if (!result.chunks) {
        fclose(fp);
        return result;
    }
    result.count = num_chunks;
    for (size_t i=0; i<num_chunks; i++) {
        size_t chunk_size = max_chunk;
        if (i == num_chunks - 1) {
            // last chunk is smaller
            chunk_size = fsize - i * max_chunk;
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

chunk_list_t chunking_results(char **input, size_t input_count) {

    chunk_list_t output= {.chunks = NULL, .count = 0};
    if (input_count == 0) {
        return output;
    }

    // initialize output.chunks.data (worstcase: every input string is its own packet)
    output.chunks = calloc(input_count, sizeof(chunk_t));
    if (!output.chunks) {
        fprintf(stderr, "malloc failde \n");
        return output;

    }
    size_t cur_len = 0;
    size_t out_idx = 0;

    output.chunks[out_idx].length = cur_len;

    output.chunks[out_idx].data = calloc(MAX_CHUNK_SIZE, sizeof(char));
    if (!output.chunks[out_idx].data) {
        fprintf(stderr, "calloc failde \n");
        free(output.chunks);
        return output;
    }
    //iterate over each input and put it in the current packet if it fits or open new packet
    for (size_t i=0; i<input_count; i++) {
        size_t len = strlen(input[i]);
        if (len >= MAX_CHUNK_SIZE) {
            // string is to big should not happen
            continue; //skip this string
        }
        if (cur_len + len < MAX_CHUNK_SIZE) {
            memcpy( output.chunks[out_idx].data + cur_len, input[i], len);
            cur_len += len;
            output.chunks[out_idx].length = cur_len;
            output.chunks[out_idx].data[cur_len] = '\0'; // nullterminate after every appending
        }
        else {
            // open new packet
            out_idx++;
            output.chunks[out_idx].data = calloc(MAX_CHUNK_SIZE, sizeof(char));
            if (!output.chunks[out_idx].data) {
                fprintf(stderr, "calloc failed\n");
                for (size_t j=0; j<out_idx; j++) {
                    free(output.chunks[j].data);
                }
                free(output.chunks);
                return output;
            }
            memcpy(output.chunks[out_idx].data, input[i], len);
            cur_len = len;
            output.chunks[out_idx].length = len;
            output.chunks[out_idx].data[cur_len] = '\0'; // nullterminate after every appending
        }
    }
    output.count = out_idx + 1; // +1 because out_idx beginns with 0 but counting begins with 0
    return output;
}
