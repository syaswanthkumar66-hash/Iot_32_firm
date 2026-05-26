#include "security.h"
#include "psa/crypto.h"
#include "esp_log.h"

static const char *TAG = "AES_GCM";

int aes_gcm_encrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *input, size_t input_len,
                    uint8_t *output, uint8_t tag[AES_GCM_TAG_LEN])
{
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attr, PSA_ALG_GCM);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

    psa_status_t status = psa_import_key(&attr, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_import_key failed: %d", (int)status);
        return -1;
    }

    size_t tag_len = AES_GCM_TAG_LEN;
    status = psa_aead_encrypt(key_id, PSA_ALG_GCM,
                              nonce, nonce_len,
                              aad, aad_len,
                              input, input_len,
                              output, input_len, &tag_len);
    psa_destroy_key(key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_aead_encrypt failed: %d", (int)status);
        return -1;
    }
    if (tag_len != AES_GCM_TAG_LEN) {
        ESP_LOGE(TAG, "tag length mismatch");
        return -1;
    }
    return 0;
}

int aes_gcm_decrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *input, size_t input_len,
                    const uint8_t *tag,
                    uint8_t *output)
{
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attr, PSA_ALG_GCM);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

    psa_status_t status = psa_import_key(&attr, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_import_key failed: %d", (int)status);
        return -1;
    }

    // GCM requires the tag to be appended to the ciphertext for decryption
    size_t ciphertext_len = input_len + AES_GCM_TAG_LEN;
    uint8_t ciphertext_with_tag[256];
    if (ciphertext_len > sizeof(ciphertext_with_tag)) {
        psa_destroy_key(key_id);
        return -1;
    }
    memcpy(ciphertext_with_tag, input, input_len);
    memcpy(ciphertext_with_tag + input_len, tag, AES_GCM_TAG_LEN);

    size_t plaintext_len = 0;
    status = psa_aead_decrypt(key_id, PSA_ALG_GCM,
                              nonce, nonce_len,
                              aad, aad_len,
                              ciphertext_with_tag, ciphertext_len,
                              output, input_len, &plaintext_len);
    psa_destroy_key(key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_aead_decrypt failed: %d", (int)status);
        return -1;
    }
    return 0;
}
