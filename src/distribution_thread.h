#ifndef DISTRIBUTION_THREAD_H
#define DISTRIBUTION_THREAD_H

#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>
#include <zmq.h>

#include "chunking.h"

typedef struct distribution_thread_args {
    void * sock;
    chunk_list_t chunks;
    int start_idx;
    int end_idx;
    char **results;
    int error;
} distribution_thread_args_t;

void *distribution_thread(void * arg);

#endif
