#include "factory_mode.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "app_config.h"
#include "security.h"

static const char *TAG = "FACTORY";

bool factory_check_entry(void) {
    // GPIO already configured in app_main, just read
    return (gpio_get_level(GPIO_BOOT_BTN) == 0);
}

void factory_mode_run(void) {
    ESP_LOGW(TAG, "Factory reset mode");
    security_reset_credentials();
    while(1) vTaskDelay(pdMS_TO_TICKS(1000));
}
