#include "relay_control.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "app_config.h"

extern bool relay_state[RELAY_COUNT];

void relay_state_save(void)
{
    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READWRITE, &handle) != ESP_OK) return;
    nvs_set_blob(handle, "relay_st", relay_state, sizeof(relay_state));
    nvs_commit(handle);
    nvs_close(handle);
}

void relay_state_load(void)
{
    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READONLY, &handle) != ESP_OK) return;
    size_t len = sizeof(relay_state);
    nvs_get_blob(handle, "relay_st", relay_state, &len);
    nvs_close(handle);
    // Apply to GPIOs without triggering NVS write
    relay_apply_state();
}
