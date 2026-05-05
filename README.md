# SPI_to_CAN

ESP32 bridge that reads a PWM-based signal and publishes status over CAN using an MCP2515 (SPI).

## What It Sends

- heartbeat frame with duty cycle, resistance estimate, and battery voltage
- error frame when PWM timing is invalid

## Hardware Roles

- ESP32 handles sensing + payload packing
- MCP2515 handles CAN framing/arbitration/CRC on the bus

## Current IDs

- `0x101` heartbeat
- `0x102` PWM error

## Build

- PlatformIO project in repo root (`featheresp32` env)

## Notes

This is still a practical prototype-style repo; scaling constants and frame maps may be tuned as hardware calibration improves.
