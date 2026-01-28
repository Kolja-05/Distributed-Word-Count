#include "protocol.h"
#include <stdbool.h>
#include <string.h>

protocol_type_t protocol_get_type(const char *message) {
    if (!message) {
        return PROTOCOL_INVALID;
    }
    if (strlen(message) < 3) {
        return PROTOCOL_INVALID;
    }
    if (strncmp(message, "map", PROTOCOL_TYPE_LEN) == 0) {
        return PROTOCOL_MAP;
    }
    if (strncmp(message, "red", PROTOCOL_TYPE_LEN) == 0) {
        return PROTOCOL_RED;
    }
    if (strncmp(message, "rip", PROTOCOL_TYPE_LEN) == 0) {
        return PROTOCOL_RIP;
    }
    return PROTOCOL_INVALID;
}



const char *protocol_get_payload(const char *message) {
    if (strlen(message) < PROTOCOL_TYPE_LEN) {
        return NULL;
    }
    return message + PROTOCOL_TYPE_LEN;
}

bool protocol_build_message(protocol_type_t type, const char *payload, char *out_buf, size_t out_buf_size) {
    const char *type_str = protocol_type_to_string(type);
    strncpy(out_buf, type_str, PROTOCOL_TYPE_LEN);
    size_t payload_len = strlen(payload);
    if (payload_len > PROTOCOL_MAX_MSG_LEN - PROTOCOL_TYPE_LEN || payload_len + PROTOCOL_TYPE_LEN > out_buf_size) {
        return false;
    }
    strncpy(out_buf + PROTOCOL_TYPE_LEN, payload, payload_len);
    out_buf[PROTOCOL_TYPE_LEN + payload_len] = '\0';
    return true;
}



bool protocol_validate_message(const char *message, size_t len) {
    if (len <= 0) {
        return false;
    }
    bool nullterminated = false;
    for (size_t i=0; i<len; i++) {
        if (message[i] == '\0') {
            nullterminated = true;
            break;
        }
    }
    if (!nullterminated) {
        return false;
    }
    // max 1500 bytes long (\0 inclusive)
    if (!(strlen(message) < PROTOCOL_MAX_MSG_LEN)) {
        return false;
    }
    return true;
}



const char *protocol_type_to_string(protocol_type_t type) {
    if (type == PROTOCOL_MAP) {
        return "map";
    }
    if (type == PROTOCOL_RED) {
        return "red";
    }
    if (type == PROTOCOL_RIP) {
        return "rip";
    }
    return "err";
}






protocol_type_t protocol_string_to_type(const char *type_str) {
    if (strncmp(type_str, "map", strlen(type_str)) == 0) {
        return PROTOCOL_MAP;
    }
    if (strncmp(type_str, "red", strlen(type_str)) == 0) {
        return PROTOCOL_RED;
    }
    if (strncmp(type_str, "rip", strlen(type_str)) == 0) {
        return PROTOCOL_RIP;
    }
    return PROTOCOL_INVALID;
}
