#include "opcodes.h"
#include "opcode_handler.h"
#include "relay_control.h"
#include "app_config.h"
#include "esp_log.h"

void opcode_dispatch(uint8_t opcode, const uint8_t *data, size_t len)
{
    switch (opcode) {
        case OPCODE_RELAY_ON:
            if (len >= 1 && data[0] < RELAY_COUNT) relay_control_set(data[0], true);
            break;
        case OPCODE_RELAY_OFF:
            if (len >= 1 && data[0] < RELAY_COUNT) relay_control_set(data[0], false);
            break;
        case OPCODE_TOGGLE:
            if (len >= 1 && data[0] < RELAY_COUNT) relay_control_toggle(data[0]);
            break;
        default:
            ESP_LOGW("OPCODE", "Unknown opcode 0x%02x", opcode);
            break;
    }
}
