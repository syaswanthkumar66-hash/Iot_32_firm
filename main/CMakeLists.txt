idf_component_register(SRCS "main.c" "app_init.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash esp_wifi esp_event esp_netif
                                 esp_https_ota esp_timer mbedtls spiffs
                                 core wifi_manager security udp_realtime
                                 opcode_protocol mqtt_client relay_control
                                 automation telemetry ota_manager storage
                                 diagnostics factory_mode cbor_codec)
