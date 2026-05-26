#pragma once
#include "esp_err.h"

typedef struct {
    uint32_t id;
    int32_t data;
} event_t;

esp_err_t event_bus_init(void);
esp_err_t event_bus_post(const event_t *evt);
esp_err_t event_bus_receive(event_t *evt, TickType_t timeout);
