#include "task_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "udp_realtime.h"
#include "mqtt_client.h"
#include "telemetry.h"
#include "diagnostics.h"
#include "app_config.h"

static const char *TAG = "TASK_MGR";

// Task handles
static TaskHandle_t wifi_task_handle = NULL;
static TaskHandle_t udp_task_handle = NULL;
static TaskHandle_t mqtt_task_handle = NULL;
static TaskHandle_t telemetry_task_handle = NULL;
static TaskHandle_t diagnostic_task_handle = NULL;

// Prototypes
static void wifi_task(void *pvParameters);
static void udp_task(void *pvParameters);
static void mqtt_task(void *pvParameters);
static void telemetry_task(void *pvParameters);
static void diagnostic_task(void *pvParameters);

void task_manager_create_tasks(void)
{
    ESP_LOGI(TAG, "Creating tasks...");

    xTaskCreatePinnedToCore(wifi_task, "wifi", 4096, NULL, 23, &wifi_task_handle, 0);
    xTaskCreatePinnedToCore(udp_task, "udp_rx", 4096, NULL, 22, &udp_task_handle, 1);
    xTaskCreatePinnedToCore(mqtt_task, "mqtt", 6144, NULL, 10, &mqtt_task_handle, 1);
    xTaskCreatePinnedToCore(telemetry_task, "telemetry", 3072, NULL, 5, &telemetry_task_handle, 1);
    xTaskCreatePinnedToCore(diagnostic_task, "diag", 3072, NULL, 1, &diagnostic_task_handle, 1);

    ESP_LOGI(TAG, "All tasks created.");
}

static void wifi_task(void *pvParameters) { wifi_manager_task(); }
static void udp_task(void *pvParameters) { udp_realtime_task(); }
static void mqtt_task(void *pvParameters) { mqtt_client_task(); }
static void telemetry_task(void *pvParameters) { telemetry_task_run(); }
static void diagnostic_task(void *pvParameters) { diagnostics_task(); }
