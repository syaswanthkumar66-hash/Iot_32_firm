#include "packet_validator.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "security.h"

static uint32_t last_second = 0;
static uint32_t packet_count = 0;
#define MAX_PACKETS_PER_SEC 30

bool packet_validator_check_rate_limit(void)
{
    uint32_t now = esp_timer_get_time() / 1000000;
    if (now != last_second) {
        last_second = now;
        packet_count = 0;
        return false; // ok
    }
    packet_count++;
    return packet_count > MAX_PACKETS_PER_SEC;
}

int packet_process(const uint8_t *data, size_t len, session_t *session, uint8_t *plain_out)
{
    // Full implementation: extract fields, verify version, decrypt, check counter, parse opcode
    // For brevity, we include essential logic:
    if (len < 1 + 4 + 4 + 12 + 16) return -1; // minimal
    const uint8_t *session_id_ptr = data + 1;
    const uint8_t *counter_ptr = data + 5;
    const uint8_t *nonce_ptr = data + 9;
    const uint8_t *payload_enc = data + 21;
    size_t payload_len = len - 21 - 16;
    const uint8_t *tag = data + len - 16;

    uint32_t counter;
    memcpy(&counter, counter_ptr, 4);
    // Replay check: sliding window (simplified)
    if (counter <= session->last_counter) {
        ESP_LOGW("PKT", "Replay packet");
        return -1;
    }
    session->last_counter = counter;

    uint8_t decrypted[256];
    int ret = aes_gcm_decrypt(session->session_key, SESSION_KEY_LEN,
                              nonce_ptr, 12,
                              NULL, 0,  // no AAD
                              payload_enc, payload_len,
                              tag, decrypted);
    if (ret != 0) {
        ESP_LOGE("PKT", "Decrypt/auth failed");
        return -1;
    }
    memcpy(plain_out, decrypted, payload_len);
    // Dispatch opcode (first byte of plain_out is opcode)
    opcode_dispatch(plain_out[0], plain_out + 1, payload_len - 1);
    return payload_len;
}
