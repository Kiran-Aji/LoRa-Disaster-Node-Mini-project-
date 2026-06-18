# Portable LoRa Disaster Communication Node

A low-cost, off-grid, and highly portable text-messaging device designed for emergency communication when traditional cellular networks and internet infrastructure fail. Built using an ESP8266 microcontroller and a LoRa radio frequency module, this node enables localized, decentralized communication during disasters.

## 🚀 Key Features
* **Off-Grid Text Messaging:** Send and receive lightweight text messages without relying on cellular data, Wi-Fi, or infrastructure.
* **Decentralized Network:** Point-to-point or mesh-capable communication tailored for localized emergency response.
* **On-Device Storage:** Built-in message logging and storage directly on the node.
* **Low Power & Compact:** Optimized for field portability and extended operation on battery power.

## 🛠️ Hardware Stack
* **Microcontroller:** ESP8266
* **RF Module:** SX1278 LoRa 
* **User Interface:** OLED Display / Serial Monitor
  
## 🔧 How to Configure and Test

This single piece of code works for both your transmitter and receiver nodes. 

### 1. Setting up the Nodes (IDs)
Before uploading the code to your two separate ESP8266 boards, you need to change the IDs at the top of the code so they can talk to each other:

* **For Board 1:** Leave it as-is (`MY_ID = "N01"` and `TARGET_ID = "N02"`).
* **For Board 2:** Swap them (`MY_ID = "N02"` and `TARGET_ID = "N01"`).

### 2. The Resistive Switch Keypad
Instead of using lots of digital pins, this project connects multiple navigation buttons to a single analog pin (**A0**) using a resistor ladder network. 

The software reads the voltage levels on pin A0 to detect which button you press:
* **Up / Down / Left / Right:** Navigates the menus and changes letters.
* **Single Click Center:** Selects a menu option or adds a letter.
* **Double Click Center:** Transmits your message over the air.
