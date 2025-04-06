# 📟 IR155-3204 + ESP32 CAN & PWM Interface

This project interfaces the **Bender IR155-3204** insulation monitoring device with an **ESP32**, using:

- **CAN Bus Communication** via MCP2515 (SPI)
- **PWM Signal Interpretation** to read insulation resistance

---

## 🛠️ Features

- 🔧 Initializes and communicates via CAN bus (MCP2515 @ 500kbps)
- 📤 Sends and receives CAN messages
- 📊 Reads PWM signal from IR155-3204 and calculates duty cycle
- 📏 Converts duty cycle to insulation resistance (example formula)
- 🔁 Modular, clean, function-based Arduino code

---

## ⚙️ Hardware Required

| Component           | Description                         |
|---------------------|-------------------------------------|
| ESP32               | Microcontroller board               |
| MCP2515 CAN Module  | CAN controller with TJA1050 transceiver |
| IR155-3204          | Insulation monitoring device        |
| Logic Level Shifter | For safe voltage interfacing (if needed) |
| Jumper Wires        | For connections                     |
| Optional: Oscilloscope | For signal verification          |

---

## 🔌 ESP32 Pin Configuration

| Signal         | ESP32 Pin |
|----------------|-----------|
| CAN CS         | GPIO 5    |
| CAN INT        | GPIO 4    |
| IR155 PWM OUT  | GPIO 15   |

---

## 📦 Library Dependencies

Install these libraries via the Arduino Library Manager:

- [`mcp_can`](https://github.com/coryjfowler/MCP_CAN_lib) by Cory J. Fowler

---

## 🚀 Getting Started

1. Wire up the MCP2515 to the ESP32 using SPI.
2. Connect the PWM output of the IR155-3204 to GPIO 15.
3. Flash the code using Arduino IDE or PlatformIO.
4. Open Serial Monitor at **115200 baud**.

---

## 📈 Example Serial Output
MCP2515 Initialized Successfully!
CAN Bus Ready!
Message Sent Successfully!
Message Received with ID: 0x100
Data: 01 02 03 04 05 06 07 08
Duty Cycle: 34.56%
Insulation Resistance: 3456.00 ohms


---

## 🧠 Notes

- The duty cycle to resistance formula should be updated based on your specific IR155-3204 model and datasheet.
- Always ensure **proper isolation** when working with high-voltage signals from the IR155-3204.

---

## ➕ Extras

- 📄 [IR155-3204 Datasheet (Bender)](https://www.bender.de/fileadmin/content/Products/Datasheets/IR155-3204_D00169_ENG.pdf)  
  Refer to this for:
  - PWM signal specs
  - Resistance vs. duty cycle mapping
  - Safety ratings and wiring

---

## 📄 License

MIT License. Feel free to use and modify.

---

## 🤝 Contributions

PRs and suggestions welcome!



