#include "security.h"
#include "psa/crypto.h"
#include "esp_log.h"

static const char *TAG = "HKDF";

esp_err_t hkdf_sha256_extract_expand(const uint8_t *salt, size_t salt_len,
                                     const uint8_t *ikm, size_t ikm_len,
                                     const uint8_t *info, size_t info_len,
                                     uint8_t *okm, size_t okm_len)
{
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_status_t status = psa_key_derivation_setup(&op, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_key_derivation_setup failed: %d", (int)status);
        return ESP_FAIL;
    }

    // Set salt (may be NULL)
    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_SALT,
                                            salt, salt_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "salt input failed: %d", (int)status);
        psa_key_derivation_abort(&op);
        return ESP_FAIL;
    }

    // Set IKM
    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_SECRET,
                                            ikm, ikm_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "ikm input failed: %d", (int)status);
        psa_key_derivation_abort(&op);
        return ESP_FAIL;
    }

    // Set info
    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_INFO,
                                            info, info_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "info input failed: %d", (int)status);
        psa_key_derivation_abort(&op);
        return ESP_FAIL;
    }

    status = psa_key_derivation_output_bytes(&op, okm, okm_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "output bytes failed: %d", (int)status);
        psa_key_derivation_abort(&op);
        return ESP_FAIL;
    }

    psa_key_derivation_abort(&op);
    return ESP_OK;
}
