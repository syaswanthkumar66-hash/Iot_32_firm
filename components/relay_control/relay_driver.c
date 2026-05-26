#include "relay_control.h"
#include "driver/gpio.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "RELAY";
static const uint8_t relay_pins[RELAY_COUNT] = { GPIO_RELAY_1, GPIO_RELAY_2, GPIO_RELAY_3, GPIO_RELAY_4 };
bool relay_state[RELAY_COUNT];
static SemaphoreHandle_t relay_mutex = NULL;

void relay_control_init(void)
{
    if (!relay_mutex) relay_mutex = xSemaphoreCreateMutex();
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_reset_pin(relay_pins[i]);
        gpio_set_direction(relay_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(relay_pins[i], 1); // off
        relay_state[i] = false;
    }
}

void relay_control_set(uint8_t relay, bool on)
{
    if (relay >= RELAY_COUNT || !relay_mutex) return;
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    relay_state[relay] = on;
    gpio_set_level(relay_pins[relay], on ? 0 : 1);
    xSemaphoreGive(relay_mutex);
    relay_state_save();
}

void relay_control_toggle(uint8_t relay)
{
    if (relay >= RELAY_COUNT || !relay_mutex) return;
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    relay_state[relay] = !relay_state[relay];
    gpio_set_level(relay_pins[relay], relay_state[relay] ? 0 : 1);
    xSemaphoreGive(relay_mutex);
    relay_state_save();
}

void relay_apply_state(void)
{
    if (!relay_mutex) return;
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_set_level(relay_pins[i], relay_state[i] ? 0 : 1);
    }
    xSemaphoreGive(relay_mutex);
}
