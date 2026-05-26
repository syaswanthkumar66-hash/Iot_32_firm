/**
 * @file main.c
 * @brief Firmware entry point
 *
 * Initializes all subsystems, starts tasks with correct core affinity and
 * priorities. All operations are reversible and safe for testing.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "app_init.h"
#include "app_config.h"
#include "task_manager.h"
#include "wifi_manager.h"
#include "security.h"
#include "udp_realtime.h"
#include "mqtt_client.h"
#include "telemetry.h"
#include "ota_manager.h"
#include "diagnostics.h"
#include "factory_mode.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    esp_err_t ret;

    // 1. Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Initialize TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Check for factory mode (GPIO0 held low at boot)
    if (factory_check_entry()) {
        factory_mode_run();
        return; // factory mode loops indefinitely
    }

    // 4. Load / generate credentials in NVS (testing mode)
    security_init_credentials();

    // 5. Initialize subsystems
    app_init();

    // 6. Create all FreeRTOS tasks
    task_manager_create_tasks();

    // 7. Start WiFi connection
    wifi_manager_start();

    ESP_LOGI(TAG, "System initialized successfully. Running tasks...");
}
