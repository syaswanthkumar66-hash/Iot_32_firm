idf_component_register(SRCS "relay_driver.c" "relay_state.c"
                       INCLUDE_DIRS "."
                                    "../../main"          # for app_config.h
                       REQUIRES esp_driver_gpio nvs_flash)
