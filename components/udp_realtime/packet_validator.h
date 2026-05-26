#pragma once
#include <stdbool.h>
#include "session_manager.h"

bool packet_validator_check_rate_limit(void);
int packet_process(const uint8_t *data, size_t len, session_t *session, uint8_t *plain_out);
