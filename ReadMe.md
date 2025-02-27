# AtomS3 Temperature Logger

This repository contains PlatformIO code for an M5 AtomS3 on a SPK base. The code reads temperature data from a DS18B20 sensor connected via a standard Grove cable, logs the readings every minute to an SD card and broadcasts the current temperature over Bluetooth. Home Assistant can discover the sensor via the ESPHome platform. The device synchronises time via Wi‑Fi and displays the current time, date, temperature (in a large font) as well as the daily maximum and minimum temperatures. An audible alert is issued once each day when the temperature exceeds 30 °C or falls below 5 °C.

## Features

- **Temperature Measurement**  
  Reads ambient temperature from a DS18B20 sensor every minute.

- **Data Logging**  
  Saves temperature readings to a daily log file on an SD card. Files are overwritten if storage space is low.

- **Bluetooth Broadcast**  
  Transmits the current temperature over BLE for Home Assistant discovery.

- **Time Synchronisation**  
  Connects to Wi‑Fi and uses NTP to maintain accurate time.

- **Display Output**  
  Shows the current time/date, temperature, and daily maximum/minimum on the AtomS3 display.

- **Speaker Alerts**  
  Issues a one-off daily alert:  
  - "over Temperature" when the temperature exceeds 30 °C  
  - "Low Temperature, put extra thick undies on" when it falls below 5 °C

- **Standard Grove Cable Wiring**  
  Uses the following standard Grove cable colours:  
  - **Red:** VDD (power, typically 3.3 V or 5 V)  
  - **Black:** GND (ground)  
  - **Yellow:** Data (connected to the DS18B20’s data pin via a 4.7 kΩ pull‑up resistor)

## Hardware Requirements

- M5 AtomS3 on a SPK base
- DS18B20 Temperature Sensor
- Standard Grove Cable
- SD Card Module (as per your SPK base specifications)
- Speaker (for audible alerts)
- Wi‑Fi Connectivity

## Wiring

### DS18B20 Sensor (via Grove Connector)

- **Red Wire:** Connect to VDD (3.3 V or 5 V, as appropriate).
- **Black Wire:** Connect to ground (GND).
- **Yellow Wire:** Connect to the sensor’s data pin (GPIO 26 on the AtomS3). A 4.7 kΩ resistor between the data line and VDD is recommended.

### SD Card

- Verify that the SD card chip-select (CS) pin (default is pin 4) matches your SPK base documentation.

### Speaker

- Connect the speaker to the designated output pin (for example, pin 10 in the code). Adjust the connection as needed for your hardware.

## Software Requirements

- PlatformIO
- M5AtomS3 library
- WiFi, NTPClient, OneWire, DallasTemperature, SD, SPI, and BLE libraries

Ensure these dependencies are installed within your PlatformIO environment.


