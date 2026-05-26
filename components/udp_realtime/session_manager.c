#include "session_manager.h"
#include <string.h>
#include "esp_log.h"
#include "esp_random.h"
#include "security.h"

#define MAX_SESSIONS 5

static session_t sessions[MAX_SESSIONS];
static int session_count = 0;

session_t *session_find(uint32_t id)
{
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].active && sessions[i].session_id == id)
            return &sessions[i];
    }
    return NULL;
}

session_t *session_create(uint32_t id, const uint8_t *remote_nonce, size_t nonce_len)
{
    if (session_count >= MAX_SESSIONS) return NULL;
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (!sessions[i].active) {
            sessions[i].active = true;
            sessions[i].session_id = id;

            uint8_t local_nonce[12];
            esp_fill_random(local_nonce, 12);
            memcpy(sessions[i].local_nonce, local_nonce, 12);

            // Correct key derivation: concatenate device_token || remote_nonce || local_nonce
            uint8_t ikm[DEVICE_TOKEN_LEN + 12 + 12];
            const uint8_t *token = security_get_device_token();
            memcpy(ikm, token, DEVICE_TOKEN_LEN);
            memcpy(ikm + DEVICE_TOKEN_LEN, remote_nonce, 12);
            memcpy(ikm + DEVICE_TOKEN_LEN + 12, local_nonce, 12);

            hkdf_sha256_extract_expand(NULL, 0,
                                       ikm, sizeof(ikm),
                                       (const uint8_t *)"session_key", 11,
                                       sessions[i].session_key, SESSION_KEY_LEN);

            session_count++;
            ESP_LOGI("SESS", "Created session %08lx", id);
            return &sessions[i];
        }
    }
    return NULL;
}

void session_destroy(uint32_t id)
{
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].session_id == id) {
            memset(&sessions[i], 0, sizeof(session_t));
            session_count--;
            break;
        }
    }
}
