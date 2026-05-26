#pragma once
#include <stdbool.h>
#include <stdint.h>

void relay_control_init(void);
void relay_control_set(uint8_t relay, bool on);
void relay_control_toggle(uint8_t relay);
void relay_state_save(void);
void relay_state_load(void);
void relay_apply_state(void);

// Accessors to replace direct extern access
void relay_state_set_all(const bool *states);
void relay_state_get_all(bool *states_out);
