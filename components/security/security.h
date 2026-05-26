#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#define DEVICE_TOKEN_LEN    32
#define SESSION_KEY_LEN     32
#define NONCE_LEN           12
#define AES_GCM_TAG_LEN     16

esp_err_t security_init_credentials(void);
void security_reset_credentials(void);
void security_export_credentials(void);
const uint8_t* security_get_device_token(void);   // NEW: accessor for session manager

esp_err_t security_init(void);

// AES-GCM encrypt/decrypt, uses hardware acceleration
int aes_gcm_encrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *input, size_t input_len,
                    uint8_t *output, uint8_t tag[AES_GCM_TAG_LEN]);

int aes_gcm_decrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *input, size_t input_len,
                    const uint8_t *tag,
                    uint8_t *output);

// HKDF-SHA256
esp_err_t hkdf_sha256_extract_expand(const uint8_t *salt, size_t salt_len,
                                     const uint8_t *ikm, size_t ikm_len,
                                     const uint8_t *info, size_t info_len,
                                     uint8_t *okm, size_t okm_len);

// Nonce manager
esp_err_t nonce_manager_init(void);
esp_err_t nonce_manager_get_counter(uint32_t *counter);
esp_err_t nonce_manager_increment_counter(void);
