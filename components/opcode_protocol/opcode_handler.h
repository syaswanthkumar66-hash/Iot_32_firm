#pragma once
#include <stdint.h>
#include <stddef.h>

void opcode_dispatch(uint8_t opcode, const uint8_t *data, size_t len);
