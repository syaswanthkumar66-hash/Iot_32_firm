#include "security.h"
#include "mbedtls/hkdf.h"
#include "esp_log.h"

static const char *TAG = "HKDF";

esp_err_t hkdf_sha256_extract_expand(const uint8_t *salt, size_t salt_len,
                                     const uint8_t *ikm, size_t ikm_len,
                                     const uint8_t *info, size_t info_len,
                                     uint8_t *okm, size_t okm_len)
{
    int ret = mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                           salt, salt_len, ikm, ikm_len,
                           info, info_len, okm, okm_len);
    if (ret != 0) {
        ESP_LOGE(TAG, "HKDF failed: %d", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}
