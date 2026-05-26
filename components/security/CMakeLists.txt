idf_component_register(SRCS "aes_gcm.c" "hkdf.c" "nonce_manager.c"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls nvs_flash esp_system)
