idf_component_register(SRCS "relay_driver.c" "relay_state.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver nvs_flash)
