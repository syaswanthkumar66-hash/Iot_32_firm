#include "relay_control.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "app_config.h"

void relay_state_save(void)
{
    bool states[RELAY_COUNT];
    relay_state_get_all(states);

    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READWRITE, &handle) != ESP_OK) return;
    nvs_set_blob(handle, "relay_st", states, sizeof(states));
    nvs_commit(handle);
    nvs_close(handle);
}

void relay_state_load(void)
{
    bool states[RELAY_COUNT] = { false };
    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READONLY, &handle) != ESP_OK) return;
    size_t len = sizeof(states);
    nvs_get_blob(handle, "relay_st", states, &len);
    nvs_close(handle);

    relay_state_set_all(states);   // apply through mutex
}
