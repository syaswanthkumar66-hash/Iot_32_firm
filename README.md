# IoTyk ESP32 Secure Relay Firmware – Testing Mode (Corrected)

All known bugs fixed: missing headers, thread‑safety, persistence, bounds checking, CMake entries.

## Quick Start

```bash
idf.py set-target esp32
idf.py menuconfig   # set WiFi credentials
idf.py build flash monitor
```

## Testing Features

- Reflashable, JTAG enabled, no eFuse.
- Credentials in encrypted NVS, resetable via factory mode (hold BOOT at power‑up).
- Serial commands: `status`, `reset_nvs`, `generate_keys`, `test_relay <n>`, `dump_sessions`.

## Production Migration

1. Edit `sdkconfig.defaults`:
   ```
   CONFIG_SECURE_BOOT_V2_ENABLED=y
   CONFIG_SECURE_FLASH_ENC_ENABLED=y
   CONFIG_SECURE_BOOT_ALLOW_JTAG=n
   CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE=y
   ```
2. Generate & burn keys **once**.
3. Rebuild & flash. JTAG disabled permanently.

See full instructions in repository root.
