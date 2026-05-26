#include "security.h"
#include "esp_system.h"
#include "esp_random.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

#define NVS_NAMESPACE "security"

static const char *TAG = "SEC";
static uint8_t device_token[DEVICE_TOKEN_LEN];
static bool initialized = false;

const uint8_t* security_get_device_token(void)
{
    return device_token;
}

esp_err_t security_init_credentials(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (ret != ESP_OK) return ret;

    size_t len = sizeof(device_token);
    ret = nvs_get_blob(handle, "dev_token", device_token, &len);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "No device token found, generating new...");
        esp_fill_random(device_token, DEVICE_TOKEN_LEN);
        nvs_set_blob(handle, "dev_token", device_token, DEVICE_TOKEN_LEN);
        nvs_commit(handle);
        ESP_LOGI(TAG, "New device token generated.");
    } else if (ret != ESP_OK) {
        nvs_close(handle);
        return ret;
    }
    nvs_close(handle);
    return ESP_OK;
}

void security_reset_credentials(void)
{
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        nvs_erase_all(handle);
        nvs_commit(handle);
        nvs_close(handle);
    }
    ESP_LOGW(TAG, "All security credentials erased.");
}

void security_export_credentials(void)
{
    printf("--- DEVICE TOKEN (hex) ---\n");
    for (int i = 0; i < DEVICE_TOKEN_LEN; i++) {
        printf("%02x", device_token[i]);
    }
    printf("\n--- END ---\n");
}

esp_err_t security_init(void)
{
    if (initialized) return ESP_OK;
    esp_err_t ret = nonce_manager_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nonce init failed");
        return ret;
    }
    initialized = true;
    ESP_LOGI(TAG, "Security subsystem ready.");
    return ESP_OK;
}
