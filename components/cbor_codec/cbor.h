#pragma once
#include <stdint.h>
#include <stddef.h>

void cbor_encode_telemetry(uint8_t *buffer, size_t buf_cap, size_t *out_len);
