# Hardware Schematic & Compatibility Matrix

This document outlines the electrical connections and hardware compatibility for **DieselDash v1.1.2**.

## 1. Pin Mapping
DieselDash is designed to plug directly into an Arduino Uno with an **LCD1602 Keypad Shield**. The following pins are utilized:


| Arduino Pin | Function | Description |
| :--- | :--- | :--- |
| **A0** | Shield Buttons | Analog reading of the 5 integrated buttons. |
| **A1** | Cylinder 1 EGT | 0-5V Input from thermocouple conditioner. |
| **A2** | Cylinder 2 EGT | 0-5V Input from thermocouple conditioner. |
| **A3** | Cylinder 3 EGT | 0-5V Input from thermocouple conditioner. |
| **A4** | Cylinder 4 EGT | 0-5V Input from thermocouple conditioner. |
| **A5** | Wideband AFR | 0-5V Input from Wideband controller. |
| **D2** | Visual Alert | Output for High-Intensity LED (via NPN transistor). |
| **D3** | Audible Alert | Output for 5V Active Piezo Buzzer. |
| **D4 - D9** | LCD Interface | Reserved for LCD communication. |
| **D10** | LCD Backlight | Controls display background light. |

---

## 2. Compatibility Matrix

The following hardware profiles are pre-mapped in the source code. Please note that while the formulas are implemented based on official manufacturer datasheets, they are currently marked as **Not Tested** until verified by a community member using the physical hardware.

### EGT Conditioners (0-5V Analog)

| Brand | Model | Status | Voltage Scale |
| :--- | :--- | :--- | :--- |
| **Innovate** | TC-4 / TC-4 Plus | **Not Tested** | 24°C – 1093°C |
| **FuelTech** | ETM-1 | **Not Tested** | 0°C – 1250°C |
| **AEM** | X-Series (EGT) | **Not Tested** | 0°C – 1000°C |
| **PLX Devices**| SM-EGT | **Not Tested** | 0°C – 1023°C |

### Wideband AFR Controllers (0-5V Analog)

| Brand | Model | Status | Voltage Scale |
| :--- | :--- | :--- | :--- |
| **Innovate** | LC-1 / LC-2 | **Not Tested** | 7.35 – 22.39 AFR |
| **AEM** | X-Series (AFR) | **Not Tested** | 8.50 – 18.00 AFR (0.5-4.5V) |

---

## 3. Wiring Logic & Failsafes

### Error Detection (Pull-down Resistors)
To ensure the system displays `ERR` if a wire vibrates loose, connect a **1MΩ resistor** between each analog input pin (**A1, A2, A3, A4, and A5**) and the Arduino **GND**. This prevents "floating" pins from providing false readings.

### High-Intensity Alarm LED (12V)
Since the Arduino pins cannot drive a high-power 12V LED directly, use an NPN transistor (e.g., PN2222):
1. **D2** -> 1kΩ Resistor -> **Base** (Middle pin) of Transistor.
2. **Emitter** pin -> **GND**.
3. **Collector** pin -> **Negative (-)** wire of the 12V LED.
4. **Positive (+)** wire of the 12V LED -> **Switched 12V DC** source.

### Grounding & Noise Reduction
**CRITICAL:** For stable readings in a diesel environment:
- Connect the GND of all sensor controllers (TC-4, LC-1, etc.) to a single chassis point.
- Connect the Arduino GND to that same point.
- This "Star Ground" prevents ground loops that cause temperature fluctuations when turning on headlights or accessories.

---

## 4. Contributing Hardware Profiles
If you have successfully tested DieselDash with a different hardware model, please submit a **Pull Request** to update this matrix and the conversion math in the `.ino` file.

