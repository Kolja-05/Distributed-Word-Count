#include "distribution_thread.h"
#include "protocol.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <zmq.h>
#include <string.h>

void *distribution_thread(void * arg) {
    distribution_thread_args_t *args = (distribution_thread_args_t *) arg;
    void * sock = args->sock;
    chunk_list_t chunks = args->chunks;
    int start_idx = args->start_idx;
    int end_idx = args->end_idx;
    protocol_type_t type = args->type;
    char **results = args->results;

    args->error = 0;
    printf("in thread\n");
    printf("start=%d, end=%d\n", start_idx, end_idx);
    for (int i=start_idx; i<end_idx; i++) {
        char msg[PROTOCOL_MAX_MSG_LEN];
        printf("for\n");
        if(!protocol_build_message(type,chunks.chunks[i].data, msg, sizeof(msg))) {
            fprintf(stderr, "error while building message\n");
            for (int j=start_idx; j<i; j++) {
                free(results[j]);
                results[j] = NULL;
            }
            args->error = 1;
            return NULL;
        }
        printf("sending message %s\n", protocol_type_to_string(type));
        zmq_send(sock, msg, strlen(msg) + 1, 0);
        // printf("------------- MAP MESSAGE ------------\n\n%s\n", msg);
        char buffer[PROTOCOL_MAX_MSG_LEN];
        int bytes_received = zmq_recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            fprintf(stderr, "recv failed from worker %d\n", i);
            for (int j=start_idx; j<i; j++) {
                free(results[j]);
                results[j] = NULL;
            }
            args->error = 2;
            return NULL;
        }
        if (!protocol_validate_message(buffer, bytes_received)) {
            fprintf(stderr, "invalid reply from worker %d\n", i);
            for (int j=start_idx; j<i; j++) {
                free(results[j]);
                results[j] = NULL;
        }
            args->error = 3;
            return NULL;
        }
        results[i] = strdup(buffer);
    }
    return NULL;
}
