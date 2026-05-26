#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "security.h" // for SESSION_KEY_LEN, DEVICE_TOKEN_LEN

typedef struct {
    bool active;
    uint32_t session_id;
    uint8_t session_key[SESSION_KEY_LEN];
    uint8_t local_nonce[12];
    uint32_t last_counter;
} session_t;

session_t *session_find(uint32_t id);
session_t *session_create(uint32_t id, const uint8_t *remote_nonce, size_t nonce_len);
void session_destroy(uint32_t id);
