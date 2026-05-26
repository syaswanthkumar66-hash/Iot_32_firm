#include "opcodes.h"
#include "opcode_handler.h"
#include <string.h>

void opcode_dispatch(uint8_t opcode, const uint8_t *data, size_t len)
{
    switch (opcode) {
        case OPCODE_RELAY_ON:
            if (len >= 1) relay_control_set(data[0], true);
            break;
        case OPCODE_RELAY_OFF:
            if (len >= 1) relay_control_set(data[0], false);
            break;
        case OPCODE_TOGGLE:
            if (len >= 1) relay_control_toggle(data[0]);
            break;
        case OPCODE_GET_STATE:
            // send state response via same session
            break;
        default:
            break;
    }
}
