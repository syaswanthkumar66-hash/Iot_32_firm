#include "security.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "NONCE";
static nvs_handle_t nvs_handle;
static uint32_t boot_counter = 0;
static uint32_t packet_counter = 0;

esp_err_t nonce_manager_init(void)
{
    esp_err_t ret = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed");
        return ret;
    }
    nvs_get_u32(nvs_handle, "boot_ctr", &boot_counter);
    nvs_get_u32(nvs_handle, "pkt_ctr", &packet_counter);
    // Increment boot counter on each boot
    boot_counter++;
    nvs_set_u32(nvs_handle, "boot_ctr", boot_counter);
    nvs_commit(nvs_handle);
    ESP_LOGI(TAG, "Boot counter: %lu", boot_counter);
    return ESP_OK;
}

esp_err_t nonce_manager_get_counter(uint32_t *counter)
{
    *counter = packet_counter;
    return ESP_OK;
}

esp_err_t nonce_manager_increment_counter(void)
{
    packet_counter++;
    nvs_set_u32(nvs_handle, "pkt_ctr", packet_counter);
    nvs_commit(nvs_handle);
    return ESP_OK;
}
