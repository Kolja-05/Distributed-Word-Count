#include <stdlib.h>
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#include "hashmap.h"
#include "protocol.h"
#include "wordcount.h"

#define WORKER_ADDR_LEN 32

int main (int argc, const char * argv[])
{
    if (argc < 2) {
        printf("Usage: %s <worker port 1> <worker port 2> ... <worker port n>", argv[0]);
        return 1;
    }
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();

    int num_ports = argc - 1;
    void *socket = zmq_socket (context, ZMQ_REP);
    bool bound = false;
    for (int i=0; i<num_ports; i++) {
        char addr[WORKER_ADDR_LEN];
        snprintf(addr,sizeof(addr), "tcp://127.0.0.1:%s",argv[i+1]);
        if(zmq_bind (socket, addr) == 0) {
            bound = true;
            break; //binded
        };
    }

    if (!bound) {
        perror("Worker could not bind to socket\n");
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return 1;
    }

    bool running = true;
    char buf[PROTOCOL_MAX_MSG_LEN];

    while (running) {
        int bytes_recieved = zmq_recv(
            socket,
            buf, 
            sizeof(buf)-1,
            0);
        if (bytes_recieved < 0) {
            continue;
        }
        buf[bytes_recieved] = '\0';
        if (!protocol_validate_message(buf)) {
            zmq_send(socket, "", 1, 0);
            continue;
        }
        protocol_type_t type = protocol_get_type(buf);

        switch (type) {
            case PROTOCOL_MAP: {
                hashmap_t *hashmap = hashmap_create();
                wordcount_map(buf + PROTOCOL_TYPE_LEN, hashmap);
                char outbuf[PROTOCOL_MAX_MSG_LEN];
                hashmap_to_string(hashmap, outbuf);
                zmq_send(socket, outbuf, strlen(outbuf),0);
                hashmap_free(hashmap);
                break;
            }
            case PROTOCOL_RED:
                zmq_send(socket, "", 1,0);
                break;
            case PROTOCOL_RIP:
                zmq_send(socket, "rip", PROTOCOL_TYPE_LEN, 0);
                running = false;
                break;

            default:
                zmq_send(socket, "", 1,0);
                break;
        }
    }
    //cleanup
    zmq_close(socket);
    zmq_ctx_destroy(context);

    return 0;
}
