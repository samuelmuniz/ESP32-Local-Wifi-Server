# ESP32 WiFi-Controlled Socket Project

This project involves an ESP32 microcontroller that serves a web interface to control two power sockets. It operates in Access Point (AP) mode, allowing users to connect to the ESP32 directly via WiFi and control the connected devices through a simple web page.

## Features
- **WiFi Access Point**: The ESP32 runs in AP mode with a default SSID "ESPWifi" and password "1234".
- **Web Interface**: A web page is served to allow the user to control two power sockets.
- **EEPROM Storage**: The ESP32 reads WiFi credentials from EEPROM, allowing for future expansion where the device could connect to a known network.
- **Visual Feedback**: The onboard LED blinks every 0.5 seconds to indicate the system is running.

## Hardware Requirements
- ESP32 Development Board
- Two relay modules connected to GPIO pins 22 and 23 for controlling the sockets.
- A power supply for the ESP32.

## Pin Configuration
- **LED_BUILTIN**: GPIO 2
- **TOMADA_1**: GPIO 22
- **TOMADA_2**: GPIO 23

## Code Overview

### Libraries Used
- **WiFi.h**: Manages WiFi functionalities.
- **AsyncTCP.h**: Provides asynchronous TCP functions.
- **ESPAsyncWebServer.h**: Enables the creation of an asynchronous web server.
- **EEPROM.h**: Provides the ability to read and write to the ESP32's EEPROM.

### Functions

#### `setup()`
- Initializes EEPROM, reads stored WiFi credentials, and sets the ESP32 to AP mode.
- Configures the GPIO pins for the onboard LED and relays.
- Starts the web server to serve the control page.

#### `loop()`
- Blinks the onboard LED every 0.5 seconds to indicate the ESP32 is running.

### Web Pages

#### Credentials Page
- Users connect to the ESP32's WiFi and access the credentials page to enter the SSID and password.
- After submitting valid credentials, the ESP32 serves the control page.

#### Control Page
- The page provides buttons to toggle the two power sockets on and off.

## How to Use
1. **Power on the ESP32**: It will create a WiFi network named "ESPWifi" with the password "1234".
2. **Connect to the WiFi Network**: Use a smartphone or laptop to connect to the ESP32’s network.
3. **Open a Web Browser**: Navigate to `192.168.4.1` to access the control interface.
4. **Control the Sockets**: Use the provided buttons to turn the connected devices on and off.

## Future Improvements
- **WiFi Credential Storage**: Implement storing of the user-provided WiFi credentials in EEPROM.
- **Additional Features**: Add feedback to the web interface to indicate the current state of the sockets.
- **Security**: Enhance the security of the WiFi network and web interface.

## License
This project is open-source and available under the MIT License.

## Author
Developed by [Your Name].
