#include "session_manager.h"
#include <string.h>
#include "esp_log.h"
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
            // derive session key: HKDF(device_token || remote_nonce || local_nonce)
            uint8_t local_nonce[12];
            esp_fill_random(local_nonce, 12);
            memcpy(sessions[i].local_nonce, local_nonce, 12);
            // For brevity, actual HKDF call omitted but implemented similarly
            hkdf_sha256_extract_expand(NULL, 0,
                                       device_token, DEVICE_TOKEN_LEN,
                                       remote_nonce, nonce_len,
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
