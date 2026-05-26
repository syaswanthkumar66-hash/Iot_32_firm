#include "relay_control.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "RELAY";

void relay_state_save(void)
{
    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READWRITE, &handle) != ESP_OK) return;
    // save relay states as blob
    nvs_commit(handle);
    nvs_close(handle);
}

void relay_state_load(void)
{
    // Load from NVS
}
