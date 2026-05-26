# IoTyk ESP32 Secure Relay Firmware – Testing Mode

**Full-featured secure relay controller with double encryption plane, local automation, and OTA – all safely reflashable.**

## Features
- AES-256-GCM encrypted UDP local control (<10ms latency)
- MQTT/TLS 1.3 cloud control with CBOR payloads
- Session key derivation via HKDF-SHA256
- Replay protection, rate limiting, auth tag verification
- Relay state persistence in encrypted NVS
- Automation rules (motion → relay)
- OTA with Ed25519 signature verification and rollback counter
- Testing mode: no eFuse, JTAG enabled, serial debug commands

## Hardware
- ESP32-WROOM-32
- 4 relays on GPIO16-19 (active LOW)
- LED on GPIO2
- Boot button on GPIO0 (hold during power-up for factory reset)

## Building
```bash
idf.py set-target esp32
idf.py menuconfig   # set WiFi SSID/Password under IoTyk Relay Firmware Configuration
idf.py build flash monitor
```

## Testing Mode Setup
- **No eFuse burning required**  
- Credentials are generated on first boot and stored in NVS (encrypted)  
- Factory reset: hold BOOT button while powering on → erases all NVS, regenerates keys  

## Serial Debug Commands
Type these in the monitor:
- `status` → heap, tasks, relay states  
- `reset_nvs` → wipe all credentials (like factory)  
- `generate_keys` → create new device token and crypto keys  
- `test_relay <n>` → toggle relay n (0-3)  
- `dump_sessions` → show active UDP sessions  

## Production Migration
1. Change `sdkconfig.defaults` as below:
```
CONFIG_SECURE_BOOT_V2_ENABLED=y
CONFIG_SECURE_FLASH_ENC_ENABLED=y
CONFIG_SECURE_BOOT_ALLOW_JTAG=n
CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y
```
2. Generate signing key:  
   `espsecure.py generate_signing_key secure_boot_key.pem`  
3. Burn keys **one time**:  
   `espefuse.py burn_key secure_boot_v2 secure_boot_key.pem`  
4. Flash and enable:  
   `idf.py bootloader` then `idf.py flash monitor`  
5. **WARNING**: This permanently locks the chip. JTAG disabled.

## Folder structure
```
iotyk_esp32/
├── CMakeLists.txt
├── sdkconfig.defaults
├── partitions.csv
├── main/
│   ├── CMakeLists.txt
│   ├── main.c
│   ├── app_init.c
│   ├── app_init.h
│   ├── app_config.h
│   └── Kconfig.projbuild
├── components/
│   ├── core/...
│   ├── wifi_manager/...
│   ├── security/...
│   ├── udp_realtime/...
│   ├── opcode_protocol/...
│   ├── mqtt_client/...
│   ├── cbor_codec/...
│   ├── relay_control/...
│   ├── automation/...
│   ├── telemetry/...
│   ├── ota_manager/...
│   ├── storage/...
│   ├── diagnostics/...
│   └── factory_mode/...
└── README.md
```
