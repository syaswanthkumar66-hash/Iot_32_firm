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
#define MQTT_BROKER_URI         CONFIG_IOTYK_MQTT_BROKER_URI
#define OTA_FIRMWARE_URL        CONFIG_IOTYK_OTA_FIRMWARE_URL

// Timing
#define WIFI_CONNECT_TIMEOUT_MS   15000
#define TELEMETRY_PERIOD_MS       10000
#define WATCHDOG_TIMEOUT_SEC      5

// UDP packet limits
#define MAX_ENCRYPTED_PACKET  256
#define MAX_PLAIN_PAYLOAD     128
