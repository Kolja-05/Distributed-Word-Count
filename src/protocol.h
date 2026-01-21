#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdbool.h>

#define PROTOCOL_MAX_MSG_LEN 1500
#define PROTOCOL_TYPE_LEN 3

typedef enum {
    PROTOCOL_MAP,
    PROTOCOL_RED,
    PROTOCOL_RIP,
    PROTOCOL_INVALID
} protocol_type_t;

protocol_type_t protocol_get_type(const char *message);

const char *protocol_get_payload(const char *message);

bool protocol_build_message(
    protocol_type_t type,
    const char *payload,
    char *out_buf,
    size_t out_buf_size
);

bool protocol_validate_message(const char *message);

const char *protocol_type_to_string(protocol_type_t type);

protocol_type_t protocol_string_to_type(const char *type_str);

size_t protocol_message_length(const char *message);

#endif
