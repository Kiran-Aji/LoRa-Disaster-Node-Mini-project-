# 📡 Portable LoRa Disaster Communication Node

A low-cost, off-grid, and highly portable text-messaging device designed for emergency communication when traditional cellular networks and internet infrastructure fail. Built using an ESP8266 microcontroller and a LoRa radio frequency module, this node enables localized, decentralized communication during disasters.

---

## 🚀 Key Features
* **Off-Grid Text Messaging:** Send and receive lightweight text messages without relying on cellular data, Wi-Fi, or infrastructure.
* **Decentralized Network:** Point-to-point communication tailored for localized emergency response.
* **Smart Screen Navigation:** Built-in menus to quickly choose between custom message typing or prebuilt emergency alerts (SOS, Medical, Food, Water).
* **Low Power & Compact:** Optimized for field portability and strict hardware layouts by reducing digital pin requirements.

---

## 🛠️ Hardware Stack
* **Microcontroller:** NodeMCU ESP8266
* **RF Module:** SX1278 LoRa (433MHz)
* **User Interface:** SSD1306 OLED Display (128x64) & Analog Resistive Keypad

### 🗺️ Pin Mapping & Wiring
Because of the GPIO limitations on the ESP8266, the hardware is configured to bypass the standard transceiver reset line, and all navigation buttons are compressed onto a single analog pin:

| Component | Module Pin | ESP8266 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **LoRa SX1278** | NSS (CS) | GPIO 15 (D8) | SPI Chip Select |
| | MOSI | GPIO 13 (D7) | SPI MOSI |
| | MISO | GPIO 12 (D6) | SPI MISO |
| | SCK | GPIO 14 (D5) | SPI Clock |
| | RST | GPIO 16 (D0) | Reset Pin |
| | DIO0 | *Unused* / -1 | Handled via software polling to save a pin |
| **OLED Display** | SDA | GPIO 4 (D2) | I2C Data |
| | SCL | GPIO 5 (D1) | I2C Clock |
| **Keypad** | Out | Analog A0 | 5-way button pad using resistor ladder |

---

## 📸 System Showcase & Demo

### 🛠️ Hardware Setup
> [Drag and drop your main hardware photo here!]

### 🎥 Working Demonstration
> [Drag and drop your working video or auto-playing GIF here to show the menus and texting in action!]

---

## 🔧 How to Configure and Test

This single piece of code works for both your transmitter and receiver nodes. 

### 1. Setting up the Nodes (IDs)
Before uploading the code to your two separate ESP8266 boards, you need to change the IDs at the top of the code so they can talk to each other:
* **For Board 1 (Node 1):** Leave it as-is:
  ```cpp
  const String MY_ID = "N01";
  const String TARGET_ID = "N02";
  
* **For Board 2 (Node 2):
  ```cpp
  const String MY_ID = "N02";
  const String TARGET_ID = "N01";
 
  
