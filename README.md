## AtomS3 Temperature Logger
This repository contains PlatformIO code for an M5 AtomS3 on a SPK base, which logs temperature data from a DS18B20 sensor connected via a standard Grove cable. The application reads and logs the temperature (in Celsius) every minute, creates daily log files on an SD card (overwriting if storage space is low), and broadcasts the current temperature via Bluetooth for Home Assistant integration through the ESPHome platform. The device also synchronises time via Wi‑Fi and displays the current time, date, current temperature (in a large font) as well as the daily maximum and minimum temperatures. Additionally, a speaker outputs a one-off daily alert if the temperature exceeds 30 °C or falls below 5 °C.

# Features
- Temperature Measurement: Reads the ambient temperature from a DS18B20 sensor every minute.
- Data Logging: Saves temperature readings to a daily log file on an SD card; files are overwritten when storage is low.
- Bluetooth Broadcast: Sends temperature data over BLE for Home Assistant detection.
- Time Synchronisation: Connects to Wi‑Fi and uses NTP for accurate timekeeping.
- Display Output: Shows the current time/date, temperature, and daily maximum/minimum readings on the AtomS3 display.
- Speaker Alerts: Delivers an audible alert with the message “over Temperature” when the temperature exceeds 30 °C, and “Low Temperature, put extra thick undies on” when it drops below 5 °C (each alert is issued only once per day).
- Standard Grove Cable Wiring: Uses standard Grove cable colours:
  - Red: VDD (power supply, typically 3.3 V or 5 V)
  - Black: GND (ground)
  - Yellow: Data (connected to the DS18B20’s data pin via a 4.7 kΩ pull‑up resistor)

# Hardware Requirements
- M5 AtomS3 on SPK base
- DS18B20 Temperature Sensor
- Standard Grove Cable
- SD Card Module (as within SPK base)
- Speaker (for audible alerts)
- Wi‑Fi Connectivity

# Wiring
# DS18B20 Sensor (via Grove Connector)
- Red Wire: Connect to VDD (3.3 V or 5 V as appropriate).
- Black Wire: Connect to GND.
- Yellow Wire: Connect to the sensor’s data pin (GPIO 26 on the AtomS3). Ensure a 4.7 kΩ pull‑up resistor is connected between the data line and VDD.

# SD Card
- Confirm that the SD card chip‐select (CS) pin (default is pin 4) corresponds with your SPK base documentation.

# Speaker
- Connect the speaker to the designated output pin (in this example, pin 10 is used). Adjust this connection as required by your hardware configuration.

# Software Requirements
- PlatformIO
- M5AtomS3 library
- WiFi, NTPClient, OneWire, DallasTemperature, SD, SPI, and BLE libraries
