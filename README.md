Hackmate

Hackmate is a versatile and modular pen-testing tool designed for educational and ethical hacking purposes. Built to operate as a "bad USB" with the Raspberry Pi Pico and to execute other advanced tasks using an ESP32, Hackmate integrates a range of tools for network and device testing, including future implementation plans for Wi-Fi deauthentication, Evil Twin attacks, NFC and RFID cloning, infrared signal manipulation, and Bluetooth jamming via overlapping RF signals. This project leverages an SH1106 display and button navigation, allowing easy menu-based access to tools and functions.
Key Features

    Bad USB Emulation: Uses a Raspberry Pi Pico to simulate keyboard inputs, automate scripts, and execute commands upon connection.
    Wi-Fi Deauthentication: Designed for testing network security by forcing clients off a Wi-Fi network.
    Future Implementations:
        Evil Twin Attack: Set up a rogue access point to capture network traffic and gather credentials from unsuspecting users.
        NFC and RFID Cloning: Develop support for reading and emulating NFC/RFID tags, adding capabilities to test access systems reliant on these technologies.
        Infrared Signal Manipulation: Implement a system for sending and capturing infrared signals, enabling testing of IR-controlled devices.
        Bluetooth Jamming: Utilize dual RFID transmitters to create intentional signal noise, interfering with Bluetooth communications.

Getting Started

This project is developed using ESP-IDF for the ESP32, so to build and run the code, make sure you have the ESP-IDF environment set up. You’ll also need to configure the Raspberry Pi Pico separately to handle the “bad USB” functionalities.
Requirements

    ESP32 with ESP-IDF installed
    Raspberry Pi Pico for bad USB emulation
    SH1106 Display for the menu interface
    Navigation Buttons for menu interaction

Installation

    Clone the Repository

    bash

git clone https://github.com/nicolasveraortiz/hackmate.git
cd hackmate

Build for ESP32
Install ESP-IDF and configure the project. Use the following commands to build and flash the code to the ESP32:

bash

    idf.py set-target esp32
    idf.py build
    idf.py -p (PORT_NAME) flash

    Set Up Raspberry Pi Pico
    Program the Raspberry Pi Pico separately for “bad USB” functionalities. Follow instructions on configuring Pico HID.

Usage

Once the ESP32 and Raspberry Pi Pico are set up, Hackmate’s menu can be accessed via the SH1106 display. The navigation buttons allow you to switch between different features:

    Select "Bad USB" to trigger keystroke injection attacks from the Raspberry Pi Pico.
    Use the Wi-Fi menu options for deauthentication tests on target networks.
    Navigate through future implementation placeholders for NFC/RFID cloning, Evil Twin, Bluetooth jamming, and infrared signal manipulation as we build these features.

Contributing

We welcome contributions to expand Hackmate’s functionality. Please follow the guidelines for pull requests, and consider proposing changes or improvements via Issues.
License

This project is open-source and available under the MIT License.