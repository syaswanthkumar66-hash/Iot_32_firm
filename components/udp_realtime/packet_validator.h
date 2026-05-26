#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "session_manager.h"

void packet_validator_init(void);
bool packet_validator_check_rate_limit(void);
int packet_process(const uint8_t *data, size_t len, session_t *session,
                   uint8_t *plain_out, size_t plain_out_max);
