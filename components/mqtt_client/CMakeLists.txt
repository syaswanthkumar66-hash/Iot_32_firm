idf_component_register(SRCS "mqtt_handler.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_mqtt cbor_codec relay_control)
