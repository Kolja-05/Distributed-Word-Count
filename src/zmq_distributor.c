#include "protocol.h"
#include "chunking.h"
#include "distribution_thread.h"
#include "chunking.h"
#include <pthread.h>
#include <stdlib.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


#define WORKER_ADDR_LEN 32

int main (int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <file.txt> <worker port 1> <worker port 2> ... <worker port n>", argv[0]);
        return 1;
    }
    int error = 0;
    void *context = zmq_ctx_new ();

    int num_workers = argc - 2;
    void *worker_socks[num_workers];
    // ------- CHUNKING -------------
    const char *filepath = argv[1];
    chunk_list_t chunks = chunking_from_file(filepath);
    char **results = calloc(chunks.count, sizeof(char *));

    // --------- CREATE WORKER SOCKETS ---------
    for (int i=0; i<num_workers; i++) {
        worker_socks[i] = zmq_socket (context, ZMQ_REQ);
        char worker_addr[WORKER_ADDR_LEN];
        snprintf(worker_addr, WORKER_ADDR_LEN, "tcp://127.0.0.1:%s", argv[i+2]);
        zmq_connect (worker_socks[i], worker_addr);
    }
    // ----------- MAP ------------
    // ----- create threads -------
    distribution_thread_args_t thread_args[num_workers];
    pthread_t threads[num_workers];
    size_t chunks_per_worker = chunks.count / num_workers;
    size_t remainding_chunks = chunks.count % num_workers;
    size_t start = 0;
    for (int i=0; i<num_workers && error == 0; i++) {
        int end = start + chunks_per_worker;
        if (i < remainding_chunks) {
            end ++;
        }
        thread_args[i].sock = worker_socks[i];
        thread_args[i].chunks = chunks;
        thread_args[i].start_idx = start;
        thread_args[i].end_idx = end;
        thread_args[i].results = results;

        start = end;

        pthread_create(&threads[i],NULL, distribution_thread, &thread_args[i]);
    }
    // ------ join threaeds ----
    for (int i=0; i<num_workers; i++) {
        pthread_join(threads[i],NULL);
    }
    // ----- check for errors from threads -----
    for (int i=0; i<num_workers; i++) {
         if (thread_args[i].error != 0) {
            fprintf(stderr, "Thread %d failed with error code %d\n", i, thread_args[i].error);
            error = 1;
        }
    }
    // ---    // ----- free results -----
    for (int i=0; i<chunks.count; i++) {
        free(results[i]);
    }
    free(results);
    //-- free chunks -----
    chunking_free(&chunks);
    // -------- RIP --------
    for (int i=0; i<num_workers; i++) {
        char buffer[PROTOCOL_MAX_MSG_LEN];
        char msg[PROTOCOL_MAX_MSG_LEN];
        if (!protocol_build_message(PROTOCOL_RIP, "", msg, sizeof(msg))) {
            fprintf(stderr, "error while building message\n");
            break;
        }
        zmq_send(worker_socks[i], msg, strlen(msg) + 1,0);
        int bytes_received = zmq_recv(worker_socks[i], buffer, PROTOCOL_MAX_MSG_LEN, 0);
        if (bytes_received < 0) {
            fprintf(stderr, "recv failed from worker %d\n", i);
            error = 1;
            break;
        }
        if (buffer[bytes_received - 1] != '\0' || !protocol_validate_message(buffer)) {
            fprintf(stderr, "invalid reply from worker %d\n", i);
            error = 1;
            break;
        }
        if (protocol_get_type(buffer) != PROTOCOL_RIP) {
            fprintf(stderr, "expected rip reply from worker %d\n", i);
            error = 1;
            break;
        }
    }
    // --------- CLEANUP --------
    for (int i=0; i<num_workers; i++) {
        zmq_close(worker_socks[i]);
    }
    zmq_ctx_destroy (context);

    if (!error) {
        return 0;
    }
    return error;
}
