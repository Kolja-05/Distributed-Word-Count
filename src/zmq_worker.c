#include <stdlib.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#include "protocol.h"

int main (int argc, const char * argv[])
{
    if (argc < 2) {
        printf("Usage: %s <port1> [port2 ...]", argv[0]);
        return 1;
    }
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();

    int num_sockets = argc - 1;
    void **sockets = malloc((num_sockets) * sizeof(void*));
    for (int i=0; i<num_sockets; i++) {
        sockets[i] = zmq_socket (context, ZMQ_REP);
        char endpoint[64];
        snprintf(endpoint,sizeof(endpoint), "tcp://127.0.0.1:%s",argv[i+1]);
        int rc = zmq_bind (sockets[i], endpoint);
        assert (rc == 0);
    }

    bool running = true;
    char buf[PROTOCOL_MAX_MSG_LEN];

    while (running) {
        for (int i=0; i<num_sockets; i++) {
            int bytes_recieved = zmq_recv(
                sockets[i], 
                buf, 
                sizeof(buf)-1,
                ZMQ_DONTWAIT);
            if (bytes_recieved < 0) {
                continue;
            }
            buf[bytes_recieved] = '\0';
            if (!protocol_validate_message(buf)) {
                zmq_send(sockets[i], "", 1, 0);
                continue;
            }
            protocol_type_t type = protocol_get_type(buf);

            switch (type) {
                case PROTOCOL_MAP:
                    zmq_send(sockets[i], "", 1,0);
                    break;
                case PROTOCOL_RED:
                    zmq_send(sockets[i], "", 1,0);
                    break;
                case PROTOCOL_RIP:
                    zmq_send(sockets[i], "rip", PROTOCOL_TYPE_LEN, 0);
                    running = false;
                    break;

                default:
                    zmq_send(sockets[i], "", 1,0);
                    break;
            }
        }
    }
    //cleanup
    for (int i=0; i<num_sockets; i++) {
        zmq_close(sockets[i]);
    }
    zmq_ctx_destroy(context);
    free(sockets);

    return 0;
}
