idf_component_register(SRCS "mqtt_handler.c"
                       INCLUDE_DIRS "."
                       REQUIRES cbor_codec relay_control)
