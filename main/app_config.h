#pragma once

// GPIO assignments
#define GPIO_RELAY_1    16
#define GPIO_RELAY_2    17
#define GPIO_RELAY_3    18
#define GPIO_RELAY_4    19
#define GPIO_LED        2
#define GPIO_BOOT_BTN   0

#define RELAY_COUNT     4

// Network
#define UDP_CONTROL_PORT        5000
#define UDP_DISCOVERY_PORT      5001
#define MQTT_BROKER_URI         "mqtts://your-broker.com"   // edit for testing
#define OTA_FIRMWARE_URL        "https://example.com/iotyk_fw.bin"

// Timing
#define WIFI_CONNECT_TIMEOUT_MS   15000
#define TELEMETRY_PERIOD_MS       10000
#define WATCHDOG_TIMEOUT_SEC      5
#define MAX_ENCRYPTED_PACKET  256
#define MAX_PLAIN_PAYLOAD     128

uint8_t plain[MAX_PLAIN_PAYLOAD];
if (len < (1+4+4+12+16) || (len - (1+4+4+12+16)) > MAX_PLAIN_PAYLOAD) {
    ESP_LOGW(TAG, "Invalid packet size %d", len);
    continue;
}
int result = packet_process(rx_buffer, len, session, plain);
