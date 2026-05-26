#include "packet_validator.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "security.h"
#include "session_manager.h"
#include <string.h>

#define MAX_PACKETS_PER_SEC 30

static uint32_t last_second = 0;
static uint32_t packet_count = 0;
static SemaphoreHandle_t rate_limit_mutex = NULL;

void packet_validator_init(void)
{
    if (!rate_limit_mutex) rate_limit_mutex = xSemaphoreCreateMutex();
}

bool packet_validator_check_rate_limit(void)
{
    xSemaphoreTake(rate_limit_mutex, portMAX_DELAY);
    uint32_t now = esp_timer_get_time() / 1000000;
    if (now != last_second) {
        last_second = now;
        packet_count = 0;
        xSemaphoreGive(rate_limit_mutex);
        return false;
    }
    packet_count++;
    bool exceeded = (packet_count > MAX_PACKETS_PER_SEC);
    xSemaphoreGive(rate_limit_mutex);
    return exceeded;
}

int packet_process(const uint8_t *data, size_t len, session_t *session,
                   uint8_t *plain_out, size_t plain_out_max)
{
    if (len < 1 + 4 + 4 + 12 + 16) return -1;
    size_t payload_len = len - (1+4+4+12+16);
    if (payload_len > plain_out_max) {
        ESP_LOGE("PKT", "Plain buffer too small");
        return -1;
    }

    const uint8_t *counter_ptr = data + 5;
    const uint8_t *nonce_ptr = data + 9;
    const uint8_t *payload_enc = data + 21;
    const uint8_t *tag = data + len - 16;

    uint32_t counter;
    memcpy(&counter, counter_ptr, 4);
    if (counter <= session->last_counter) {
        ESP_LOGW("PKT", "Replay packet");
        return -1;
    }
    session->last_counter = counter;

    uint8_t decrypted[MAX_ENCRYPTED_PACKET];
    int ret = aes_gcm_decrypt(session->session_key, SESSION_KEY_LEN,
                              nonce_ptr, 12, NULL, 0,
                              payload_enc, payload_len, tag, decrypted);
    if (ret != 0) {
        ESP_LOGE("PKT", "Decrypt/auth fail");
        return -1;
    }
    memcpy(plain_out, decrypted, payload_len);

    // Increment global nonce counter
    nonce_manager_increment_counter();

    // Dispatch opcode (first byte)
    opcode_dispatch(decrypted[0], decrypted + 1, payload_len - 1);
    return (int)payload_len;
}
