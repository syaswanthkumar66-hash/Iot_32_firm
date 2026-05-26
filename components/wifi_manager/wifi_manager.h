#pragma once
#include "freertos/event_groups.h"
extern EventGroupHandle_t wifi_event_group;
extern const int WIFI_CONNECTED_BIT;

void wifi_manager_start(void);
void wifi_manager_task(void);
