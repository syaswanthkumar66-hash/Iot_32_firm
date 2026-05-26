#pragma once
#include <stdbool.h>
#include <stdint.h>

void relay_control_init(void);
void relay_control_set(uint8_t relay, bool on);
void relay_control_toggle(uint8_t relay);
void relay_state_save(void);
void relay_state_load(void);
void relay_apply_state(void);   // apply loaded state to GPIOs without NVS write
