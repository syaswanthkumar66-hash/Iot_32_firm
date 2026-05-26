#include "security.h"
#include "mbedtls/gcm.h"
#include "esp_log.h"

static const char *TAG = "AES_GCM";

int aes_gcm_encrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *input, size_t input_len,
                    uint8_t *output, uint8_t tag[AES_GCM_TAG_LEN])
{
    mbedtls_gcm_context ctx;
    mbedtls_gcm_init(&ctx);
    int ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, key_len * 8);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_gcm_setkey failed: %d", ret);
        mbedtls_gcm_free(&ctx);
        return ret;
    }
    ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT,
                                    input_len, nonce, nonce_len,
                                    aad, aad_len, input, output,
                                    AES_GCM_TAG_LEN, tag);
    mbedtls_gcm_free(&ctx);
    return ret;
}

int aes_gcm_decrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *input, size_t input_len,
                    const uint8_t *tag,
                    uint8_t *output)
{
    mbedtls_gcm_context ctx;
    mbedtls_gcm_init(&ctx);
    int ret = mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, key_len * 8);
    if (ret != 0) {
        ESP_LOGE(TAG, "setkey failed");
        mbedtls_gcm_free(&ctx);
        return ret;
    }
    ret = mbedtls_gcm_auth_decrypt(&ctx, input_len, nonce, nonce_len,
                                   aad, aad_len, tag, AES_GCM_TAG_LEN,
                                   input, output);
    mbedtls_gcm_free(&ctx);
    return ret;
}
