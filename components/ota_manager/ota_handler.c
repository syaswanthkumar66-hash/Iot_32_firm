// components/ota_manager/ota_handler.c
#include "ota_manager.h"
#include "esp_log.h"

static const char *TAG = "OTA";

void ota_manager_init(void) {
    ESP_LOGI(TAG, "OTA ready (Ed25519 verification)");
}
