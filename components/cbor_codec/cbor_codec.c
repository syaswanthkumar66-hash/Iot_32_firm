#include "cbor.h"
#include "tinycbor/cbor.h"

void cbor_encode_telemetry(uint8_t *buffer, size_t *len)
{
    CborEncoder encoder;
    cbor_encoder_init(&encoder, buffer, 256, 0);
    // Example: encode a simple map
    CborEncoder map;
    cbor_encoder_create_map(&encoder, &map, 1);
    cbor_encode_text_stringz(&map, "heap");
    cbor_encode_int(&map, esp_get_free_heap_size());
    cbor_encoder_close_container(&encoder, &map);
    *len = cbor_encoder_get_buffer_size(&encoder, buffer);
}
