#include "storage.h"
#include "nvs.h"
#include "esp_log.h"

void storage_init(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("STOR", "Failed to open NVS namespace");
    } else {
        nvs_close(handle);
        ESP_LOGI("STOR", "Storage namespace ready.");
    }
}
