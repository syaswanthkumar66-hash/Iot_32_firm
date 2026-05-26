#include "relay_control.h"
#include "driver/gpio.h"
#include "app_config.h"

static const uint8_t relay_pins[RELAY_COUNT] = {
    GPIO_RELAY_1, GPIO_RELAY_2, GPIO_RELAY_3, GPIO_RELAY_4
};
static bool relay_state[RELAY_COUNT];

void relay_control_init(void)
{
    for (int i = 0; i < RELAY_COUNT; i++) {
        gpio_reset_pin(relay_pins[i]);
        gpio_set_direction(relay_pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(relay_pins[i], 1); // active low, off
        relay_state[i] = false;
    }
}

void relay_control_set(uint8_t relay, bool on)
{
    if (relay >= RELAY_COUNT) return;
    relay_state[relay] = on;
    gpio_set_level(relay_pins[relay], on ? 0 : 1);
    relay_state_save();
}

void relay_control_toggle(uint8_t relay)
{
    if (relay >= RELAY_COUNT) return;
    relay_state[relay] = !relay_state[relay];
    gpio_set_level(relay_pins[relay], relay_state[relay] ? 0 : 1);
    relay_state_save();
}
