idf_component_register(SRCS "task_manager.c" "event_bus.c"
                       INCLUDE_DIRS "."
                       REQUIRES freertos esp_system)
