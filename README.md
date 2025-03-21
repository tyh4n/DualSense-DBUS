# DualSense to SBUS/DBUS Converter

This project uses an **ESP32** to connect to a **PS5 DualSense controller** via Bluetooth and convert its joystick inputs into **SBUS/DBUS** signals. These signals can be used to control RC vehicles, drones, or other SBUS/DBUS-compatible devices.

## Table of Contents
1. [Overview](#overview)
2. [Libraries](#libraries)
3. [Code Explanation](#code-explanation)
4. [Wiring](#wiring)

## Overview

The ESP32 reads joystick inputs from the DualSense controller and maps them to SBUS/DBUS channels. The joystick values (ranging from `-128` to `128`) are converted to SBUS/DBUS values (`0` to `2047`) and sent over the serial port.

## Libraries

This project uses the following libraries:

- **BMC_SBUS**: For generating SBUS signals.  
  [GitHub Repository](https://github.com/boldstelvis/BMC_SBUS)

- **RM_DBUS**: For generating DBUS signals.  
  [GitHub Repository](https://github.com/your-repo-link)

- **ps5Controller**: For connecting to and reading inputs from the DualSense controller.  
  [GitHub Repository](https://github.com/rodneybakiskan/ps5-esp32)

- **FUTABA_SBUS**: Original SBUS library by Michael Baker.  
  [GitHub Repository](https://github.com/mikeshub/FUTABA_SBUS)

- **SBUS Protocol**: Initial reverse engineering by Uwe Gartmann.  
  [Notebook](https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/)

## Code Explanation

The code reads joystick inputs from the DualSense controller and maps them to SBUS/DBUS channels.

1. **Setup**:
   - Initialize the SBUS/DBUS object.
   - Start Bluetooth communication with the DualSense controller.

2. **Loop**:
   - Read joystick and button inputs from the DualSense controller.
   - Map joystick values (`-128` to `128`) to SBUS/DBUS values (`0` to `2047`).
   - Send the mapped values to the SBUS/DBUS channels.
   - Transmit the SBUS/DBUS data over the serial port.

## Wiring

### Connections:
- **ESP32 TX Pin17** → **SBUS/DBUS Receiver RX Pin**
- **ESP32 GND** → **SBUS/DBUS Receiver GND**
- **ESP32 3.3V** → **SBUS/DBUS Receiver VCC**

### Wiring:
![Wiring](wiring.png)

---