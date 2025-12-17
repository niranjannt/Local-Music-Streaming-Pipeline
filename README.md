# Local Music Streaming Pipeline (ECE 445L Final Project)

This repository contains the final project for **Embedded Design Laboratory (ECE 445L)** at **UT Austin**, developed by Niranjan Telkikar, Ashton Knecht, Thompson Truong, and Nathan DeLaRosa. The project implements a music-streaming pipeline using local WAV files, combining frontend/backend software with embedded hardware.

## Project Overview

Our system streams music from a web interface to an **ESP32-Devkit-V1** and outputs audio through a **12-bit SPI DAC** connected to a headphone jack. The pipeline includes both **analog and digital filters** controlled in real-time by circular potentiometers, allowing adjustment of treble, mid, bass, and digital effects such as flanger, distortion, and reverb.

### Pipeline Flow

1. **Web Interface**: Streams WAV files via Java Serial library to the ESP32 through a USB-UART converter.  
2. **ESP32**: Receives serial data and sends it to the SPI DAC (mono channel). It also handles digital filter adjustments.  
3. **DAC**: Outputs analog audio to headphones.  
4. **TM4C123GH6PM**: Reads potentiometer inputs for analog filters and adjusts MOSFET gain circuits for treble, mid, and bass channels in real-time.  
5. **Visualizer**: Displays right-bass changes in real-time using ADC input from the TM4C.

---

## Team Contributions

- **Ashton Knecht**:  
  - Designed and soldered the PCB with ESP32-Devkit-V1.  
  - Implemented MOSFET gain circuits for analog filters.  
  - Carefully routed PCB to minimize trace length.

- **Thompson Truong**:  
  - Designed the enclosure and circular potentiometers for analog and digital filter control.  
  - Developed ESP-IDF code for digital filters on the ESP32 and analog filters on the TM4C.

- **Nathan DeLaRosa**:  
  - Implemented audio playback and backend/frontend for the web interface.  
  - Wrote ESP-IDF code for streaming audio.

- **Niranjan Telkikar**:  
  - Wrote and tested the **mono DAC_Out function** for the ESP32 (Arduino IDE/ESP-IDF).  
  - Developed a basic visualizer for real-time bass monitoring.  
  - Assisted with hardware debugging and ESP-IDF integration.

---

## Hardware Components

- ESP32-Devkit-V1  
- TM4C123GH6PM microcontroller  
- 12-bit SPI DAC (mono)  
- MOSFET-based analog gain circuits  
- Circular potentiometers for analog and digital filters  
- Headphone jack for output audio

---

## Software Components

- Arduino IDE / ESP-IDF framework for microcontroller programming  
- Java Serial library for web-to-ESP32 streaming  
- Frontend/backend web interface for music streaming  
- Real-time visualizer reading ADC inputs from TM4C

---

## Usage

1. Connect the ESP32 and TM4C to your computer via USB.  
2. Launch the web interface to stream WAV files.  
3. Adjust analog and digital filters using the circular potentiometers.  
4. Observe audio output through headphones and visualizer display.

---

## Acknowledgements

This project was completed as part of **ECE 445L: Embedded Design Laboratory** at **UT Austin**.

