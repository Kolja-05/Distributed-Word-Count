#include "protocol.h"
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
    // --------- CREATE WORKER SOCKETS ---------
    for (int i=0; i<num_workers; i++) {
        worker_socks[i] = zmq_socket (context, ZMQ_REQ);
        char worker_addr[WORKER_ADDR_LEN];
        snprintf(worker_addr, WORKER_ADDR_LEN, "tcp://127.0.0.1:%s", argv[i+2]);
        zmq_connect (worker_socks[i], worker_addr);
    }
    // ----------- MAP ------------
    for (int i=0; i<num_workers; i++) {
        char buffer [PROTOCOL_MAX_MSG_LEN];
        char msg[PROTOCOL_MAX_MSG_LEN];
        if (!protocol_build_message(PROTOCOL_MAP, "", msg, sizeof(msg))) {
            fprintf(stderr, "error while building message\n");
            error = 1;
            break;
        }
        zmq_send (worker_socks[i], msg, strlen(msg) + 1, 0);
        int bytes_received = zmq_recv (worker_socks[i], buffer, PROTOCOL_MAX_MSG_LEN, 0);
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
    }
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
    return 1;
}
