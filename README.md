# SPI_to_CAN

Simple version: this project reads a PWM signal from a Bender IR155-3204, estimates insulation resistance, reads battery voltage, and sends that data over CAN.

The ESP32 talks to an MCP2515 over SPI, and the MCP2515 handles the CAN framing on the bus.

## What It Does

- Reads IR155 PWM on `GPIO15`
- Computes duty cycle
- Converts duty cycle to resistance (current formula is a project-side model)
- Reads battery voltage using ADC
- Sends a heartbeat CAN message (`ID 0x101`)
- Sends a PWM error CAN message (`ID 0x102`) if PWM timing is invalid
- Also listens for incoming CAN frames and prints them to serial

## Quick Hardware Map

- `ESP32`
- `MCP2515 + CAN transceiver` (SPI CAN interface)
- `IR155-3204` PWM output

ESP32 pins in this repo:

- `CAN CS` -> `GPIO5`
- `CAN INT` -> `GPIO4`
- `PWM IN` -> `GPIO15`
- `Battery ADC` -> `A0`

## How The Data Flow Works

1. ESP32 measures PWM high/low time using `pulseIn()`.
2. It calculates duty cycle.
3. It converts duty cycle to resistance.
4. It samples battery voltage.
5. It packs those values into CAN payload bytes.
6. It sends the frame through MCP2515 (over SPI).
7. In parallel, it checks MCP2515 INT for received CAN frames and prints them.

The main loop runs this every ~250 ms.

## Heartbeat Payload (ID `0x101`)

Current transmit layout:

- Byte 0: `0xAA` (heartbeat marker)
- Byte 1: duty cycle percent as integer (`0..100`)
- Byte 2-3: resistance (int16 style split high/low)
- Byte 4-5: battery voltage x100 (for example `12.34V` -> `1234`)

Error frame (`0x102`) currently sends:

- `0xEE 0x01` when PWM measurement fails

## CAN In Plain English

CAN is a shared two-wire bus (`CAN_H`, `CAN_L`) used all over automotive systems.

- There is no master node.
- Every ECU can talk when the bus is idle.
- Every ECU hears every frame.
- Priority is determined by CAN ID (lower numeric ID = higher priority).
- If two nodes start at the same time, arbitration resolves it without trashing the winning frame.

Why automotive likes CAN:

- Good noise immunity (differential signaling)
- Predictable arbitration behavior
- Built-in error detection and retransmission
- Message-based (you care about signal IDs, not point-to-point links)

## CAN Frame Order (Classic CAN 2.0, Standard 11-bit ID)

This is the order on the wire for a normal data frame:

1. `SOF` (1 bit): start of frame (dominant `0`)
2. `Identifier` (11 bits): message priority + meaning
3. `RTR` (1 bit): `0` for data frame, `1` for remote request
4. `IDE` (1 bit): `0` for standard 11-bit ID frame
5. `r0` (1 bit): reserved
6. `DLC` (4 bits): payload length code (`0..8` bytes in classic CAN)
7. `Data` (0..8 bytes)
8. `CRC sequence` (15 bits): checksum over frame content
9. `CRC delimiter` (1 bit): recessive delimiter
10. `ACK slot` (1 bit): receivers drive dominant if frame was valid
11. `ACK delimiter` (1 bit): recessive delimiter
12. `EOF` (7 bits): end of frame
13. `IFS` / intermission (3 bits): gap before next frame

Notes:

- Bit stuffing is used from `SOF` through the `CRC sequence` (to keep clock sync).
- CRC in classic CAN is a 15-bit CRC (handled by CAN controller hardware, here MCP2515).

## Who Handles What In This Repo

- ESP32 code decides CAN ID + payload bytes.
- MCP2515 handles low-level CAN protocol details (CRC generation/checking, bit timing, arbitration participation, ACK behavior).
- MCP_CAN library provides the API layer used in `src/main.cpp`.

## Build / Run

PlatformIO environment in this repo:

- Board: `featheresp32`
- Framework: `arduino`
- Serial: `115200`
- CAN library: [`MCP_CAN_lib`](https://github.com/coryjfowler/MCP_CAN_lib)

Basic steps:

1. Wire ESP32 + MCP2515 + CAN transceiver + IR155 PWM.
2. Build and flash.
3. Open serial monitor at `115200`.
4. Check for:
   - `MCP2515 Initialized`
   - `CAN Bus active`
   - periodic duty/resistance/voltage output

## Practical Automotive Context

If you think in ECU terms, this node is basically a small sensor ECU:

- It converts physical measurements (insulation monitor PWM + battery ADC)
- It publishes those as CAN frames for other ECUs/tools
- Other nodes decide what to do with the data (log, warn, shut down, etc.)

Same pattern scales from student projects to real vehicle networks.

## References

- Bender IR155-3204 datasheet: https://www.bender.de/fileadmin/content/Products/Datasheets/IR155-3204_D00169_ENG.pdf
- CAN frame basics (overview): https://www.peak-system.com/know-how/technologies/can/
- CAN CRC overview (CiA): https://can-cia.org/can-knowledge/cyclic-redundancy-check-crc-in-can-frames

## License

MIT
