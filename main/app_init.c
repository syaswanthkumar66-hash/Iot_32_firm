#include "app_init.h"
#include "esp_log.h"
#include "security.h"
#include "relay_control.h"
#include "automation.h"
#include "storage.h"
#include "telemetry.h"
#include "ota_manager.h"
#include "mqtt_client.h"
#include "udp_realtime.h"
#include "event_bus.h"
#include "packet_validator.h"

static const char *TAG = "APP_INIT";

void app_init(void)
{
    ESP_LOGI(TAG, "Initializing application components...");

    // Event bus for inter-component communication
    event_bus_init();

    // Storage (load saved states)
    storage_init();

    // Relay driver and state (state load does NOT trigger NVS writes)
    relay_control_init();
    relay_state_load();

    // Rate limiter mutex
    packet_validator_init();

    // Automation engine
    automation_init();

    // Security (load keys from NVS, init nonce managers)
    security_init();

    // Telemetry
    telemetry_init();

    // OTA manager (prepare callback)
    ota_manager_init();

    // MQTT client (will start later after IP acquired)
    mqtt_client_init();

    // UDP realtime server (will bind after IP acquired)
    udp_realtime_init();

    ESP_LOGI(TAG, "Application components ready.");
}
