#include "chunking.h"
#include "string.h"
#include "stdlib.h"
#include <ctype.h>
#include <stdio.h>




chunk_list_t chunking_from_file(const char * filepath) {
    chunk_list_t result = {.chunks = NULL, .count = 0};
    FILE * fp = fopen(filepath, "r");
    if (!fp) {
        return result;
    }
    // estimate the chunk size to reduce realloc calls
    fseek(fp, 0, SEEK_END);
    long fsize_long = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fsize_long < 0) {
        fclose(fp);
        return result;
    }
    size_t fsize = (size_t) fsize_long;
    size_t max_chunk = (size_t) MAX_CHUNK_SIZE;
    size_t estimated_chunks = fsize / max_chunk;
    size_t remaining_bytes = fsize % (size_t) max_chunk;

    if (remaining_bytes > 0) {
        estimated_chunks ++;
    }


    result.chunks = malloc(sizeof(chunk_t) * estimated_chunks);
    if (!result.chunks) {
        fclose(fp);
        return result;
    }

    char *buf = malloc(MAX_CHUNK_SIZE+1);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        fclose(fp);
        free(result.chunks);
        return result;
    }
    size_t buf_len = 0;
    size_t last_seperator = 0;
    int c; // current character, mus be int, to detect EOF
    while((c=fgetc(fp)) != EOF) {
        buf[buf_len] = (char)c;
        buf_len ++;
        if (!isalpha(c)) {
            last_seperator = buf_len;
        }
        if (buf_len == MAX_CHUNK_SIZE) {
            size_t chunk_len = last_seperator; // if there was no seperator word is to big TODO error, but should not happen since words are shorter than 1496 characters
            // save chunk
            // check if space is in dynamic array
            if (result.count >= estimated_chunks) {
                estimated_chunks = estimated_chunks *2; // Double
                chunk_t * tmp = realloc(result.chunks, estimated_chunks * sizeof(chunk_t));
                if (!tmp) {
                    free(buf);
                    for (size_t i=0; i<result.count; i++) {
                        free(result.chunks[i].data);
                    }
                    free(result.chunks);
                    fclose(fp);
                    chunk_list_t result = {.chunks = NULL, .count = 0};
                    return result;
                }
                result.chunks = tmp;
            }
            chunk_t *chunk = &result.chunks[result.count];
            result.count ++;
            chunk->data = malloc(chunk_len + 1);
            if (!chunk->data) {
                free(buf);
                for (size_t i=0; i<result.count; i++) free(result.chunks[i].data);
                free(result.chunks);
                fclose(fp);
                chunk_list_t result = {.chunks = NULL, .count = 0};
                return result;
            }
            memcpy(chunk->data, buf, chunk_len);
            chunk->data[chunk_len] = '\0';
            chunk->length = chunk_len;

            size_t remaining_len = buf_len - chunk_len;
            memmove(buf, buf + chunk_len, remaining_len);
            buf_len = remaining_len;

            //find last seperator
            last_seperator = 0;
            for (int i=0; i<buf_len; i++) {
                if (!isalpha((unsigned char) buf[i])) {
                    last_seperator = i+1;
                }
            }
        }
    }
    // last chunk
    if(buf_len > 0) {
        // check if space is in dynamic array
        if (result.count >= estimated_chunks) {
            estimated_chunks = estimated_chunks *2; // Double
            chunk_t * tmp = realloc(result.chunks, estimated_chunks * sizeof(chunk_t));
            if (!tmp) {
                free(buf);
                for (size_t i=0; i<result.count; i++) {
                    free(result.chunks[i].data);
                }
                free(result.chunks);
                fclose(fp);
                chunk_list_t result = {.chunks = NULL, .count = 0};
                return result;
            }
            result.chunks = tmp;
        }
        chunk_t *chunk = &result.chunks[result.count];
        result.count ++;
        chunk->data = malloc(buf_len + 1);
        if (!chunk->data) {
            free(buf);
            for (size_t i=0; i<result.count; i++) free(result.chunks[i].data);
            free(result.chunks);
            fclose(fp);
            chunk_list_t result = {.chunks = NULL, .count = 0};
            return result;
        }
        memcpy(chunk->data, buf, buf_len);
        chunk->data[buf_len] = '\0';
        chunk->length = buf_len;
    }
    free(buf);
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
