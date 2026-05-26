#include "relay_control.h"
#include "driver/gpio.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <string.h>
#include <assert.h>

static const char *TAG = "RELAY";
static const uint8_t relay_pins[RELAY_COUNT] = {
    GPIO_RELAY_1, GPIO_RELAY_2, GPIO_RELAY_3, GPIO_RELAY_4
};
static bool relay_state[RELAY_COUNT];
static SemaphoreHandle_t relay_mutex = NULL;

// Internal save that does NOT take the mutex (caller must hold it)
static void relay_state_save_internal(const bool *states)
{
    nvs_handle_t handle;
    if (nvs_open("storage", NVS_READWRITE, &handle) != ESP_OK) return;
    nvs_set_blob(handle, "relay_st", states, sizeof(relay_state));
    nvs_commit(handle);
    nvs_close(handle);
}

void relay_control_init(void)
{
    relay_mutex = xSemaphoreCreateMutex();
    assert(relay_mutex != NULL);               // loud failure if OOM
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_reset_pin(relay_pins[i]);
        gpio_set_direction(relay_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(relay_pins[i], 1); // off
        relay_state[i] = false;
    }
}

void relay_control_set(uint8_t relay, bool on)
{
    assert(relay_mutex);                       // must be initialised
    if (relay >= RELAY_COUNT) return;

    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    relay_state[relay] = on;
    gpio_set_level(relay_pins[relay], on ? 0 : 1);

    // Save now while still holding the mutex – atomic with GPIO change
    relay_state_save_internal(relay_state);
    xSemaphoreGive(relay_mutex);
}

void relay_control_toggle(uint8_t relay)
{
    assert(relay_mutex);
    if (relay >= RELAY_COUNT) return;

    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    relay_state[relay] = !relay_state[relay];
    gpio_set_level(relay_pins[relay], relay_state[relay] ? 0 : 1);

    relay_state_save_internal(relay_state);
    xSemaphoreGive(relay_mutex);
}

void relay_apply_state(void)
{
    assert(relay_mutex);
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_set_level(relay_pins[i], relay_state[i] ? 0 : 1);
    }
    xSemaphoreGive(relay_mutex);
}

void relay_state_set_all(const bool *states)
{
    assert(relay_mutex);
    if (!states) return;
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    memcpy(relay_state, states, sizeof(relay_state));
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_set_level(relay_pins[i], relay_state[i] ? 0 : 1);
    }
    // Do NOT save here – callers must decide when to persist
    xSemaphoreGive(relay_mutex);
}

void relay_state_get_all(bool *states_out)
{
    assert(relay_mutex);
    if (!states_out) return;
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    memcpy(states_out, relay_state, sizeof(relay_state));
    xSemaphoreGive(relay_mutex);
}

// Public save – still available for explicit external saves
void relay_state_save(void)
{
    assert(relay_mutex);
    xSemaphoreTake(relay_mutex, portMAX_DELAY);
    relay_state_save_internal(relay_state);
    xSemaphoreGive(relay_mutex);
}
