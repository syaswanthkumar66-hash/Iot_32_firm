#include "telemetry.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_config.h"

static const char *TAG = "TELEM";

void telemetry_init(void) {}
void telemetry_task_run(void) {
    while(1) {
        ESP_LOGI(TAG, "Telemetry: heap=%d", esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_PERIOD_MS));
    }
}
