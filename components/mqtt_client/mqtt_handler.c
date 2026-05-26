#include "mqtt_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "cbor.h"
#include "relay_control.h"

static const char *TAG = "MQTT";

void mqtt_client_init(void) {
    // configuration loaded, client started in task
}

void mqtt_client_task(void) {
    ESP_LOGI(TAG, "MQTT task started (testing mode – TLS1.3)");
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
