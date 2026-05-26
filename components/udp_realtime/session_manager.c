#include "session_manager.h"
#include <string.h>
#include "esp_log.h"
#include "esp_random.h"
#include "security.h"

#define MAX_SESSIONS 5
static session_t sessions[MAX_SESSIONS];
static int session_count = 0;

__attribute__((warn_unused_result)) session_t *session_find(uint32_t id)
{
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (sessions[i].active && sessions[i].session_id == id)
            return &sessions[i];
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

            // Build IKM: device_token || remote_nonce || local_nonce
            const uint8_t *token = security_get_device_token();
            volatile uint8_t ikm[DEVICE_TOKEN_LEN + 12 + 12];
            memcpy((void *)ikm, token, DEVICE_TOKEN_LEN);
            memcpy((void *)(ikm + DEVICE_TOKEN_LEN), remote_nonce, 12);
            memcpy((void *)(ikm + DEVICE_TOKEN_LEN + 12), local_nonce, 12);

            hkdf_sha256_extract_expand(NULL, 0,
                                       (const uint8_t *)ikm, sizeof(ikm),
                                       (const uint8_t *)"session_key_v1", 15,
                                       sessions[i].session_key, SESSION_KEY_LEN);
            memset((void *)ikm, 0, sizeof(ikm)); // clear volatile to prevent optimization

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
