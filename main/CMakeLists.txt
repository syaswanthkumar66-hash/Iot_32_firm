idf_component_register(SRCS "main.c" "app_init.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash esp_wifi esp_event esp_netif
                                 esp_https_ota esp_timer mbedtls
                                 spiffs json cbor)
