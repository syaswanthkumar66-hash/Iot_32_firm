#include "diagnostics.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "DIAG";
void diagnostics_task(void) {
    while(1) {
        ESP_LOGI(TAG, "Watermark: %d", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}
