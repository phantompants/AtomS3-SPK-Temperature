#include <M5AtomS3.h>   // Use the AtomS3 header to resolve "M5Atom.h: No such file" error
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <SPI.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

// ---------------------------------------------------------------------------
// Grove Cable Wiring for DS18B20 Sensor
// ---------------------------------------------------------------------------
// Standard Grove cable wiring colours:
//   - Red: VDD (3.3V or 5V)
//   - Black: GND
//   - Yellow: Data (connects to DS18B20 DQ pin; requires a 4.7kΩ resistor between Data and VDD)
// ---------------------------------------------------------------------------

// Define pin for DS18B20 sensor data (via Grove connector)
#define DS18B20_PIN 26

// Define SD card chip‐select pin (verify with your SPK base documentation)
#define SD_CS_PIN 4

// Wi‑Fi credentials (replace with your network details)
#define WIFI_SSID     "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// NTP settings (adjust NTP_OFFSET as per your timezone)
#define NTP_OFFSET 3600    // e.g. GMT+1
#define NTP_SERVER "pool.ntp.org"

// BLE service and characteristic UUIDs (example values)
#define SERVICE_UUID         "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID  "abcdefab-1234-1234-1234-abcdefabcdef"

// Global objects and variables
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET, 60000); // update every minute

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

float currentTemp = 0.0;
float maxTemp = -1000.0;
float minTemp = 1000.0;

BLECharacteristic *pCharacteristic = nullptr;

unsigned long previousMillis = 0;
const long interval = 60000; // 60 seconds

// Variables to ensure the speaker alerts occur only once per day
int currentDay = -1;
bool highTempAlertDone = false;
bool lowTempAlertDone = false;

// ---------------------------------------------------------------------------
// Placeholder function for speaker output.
// Replace this with your text-to-speech or audio playback method as required.
// ---------------------------------------------------------------------------
void speakMessage(const char* message) {
  // For now, simply print the message to Serial and simulate a beep pattern.
  Serial.println(message);
  // Example: beep three times (modify if you have a proper TTS solution)
  for (int i = 0; i < 3; i++) {
    // The following tone() call assumes you have access to a speaker pin.
    // Replace SPEAKER_PIN with the correct pin number if necessary.
    tone(10, 1000, 200);  // 1000 Hz tone for 200 ms on pin 10 (adjust as needed)
    delay(300);
  }
}

// ---------------------------------------------------------------------------
// Update the AtomS3 display with time/date, current temperature, and daily max/min.
// ---------------------------------------------------------------------------
void updateDisplay() {
  M5.dis.display.clear();

  // Retrieve time/date from the NTP client
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = localtime(&epochTime);
  char dateStr[11];
  sprintf(dateStr, "%04d-%02d-%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
  String timeStr = timeClient.getFormattedTime(); // Format: HH:MM:SS

  // Display time and date (small font)
  M5.dis.setTextSize(1);
  M5.dis.setCursor(0, 0);
  M5.dis.printf("%s %s\n", dateStr, timeStr);

  // Display current temperature in a larger font
  M5.dis.setTextSize(3);
  M5.dis.setCursor(0, 20);
  M5.dis.printf("T: %.1f C\n", currentTemp);

  // Display daily maximum and minimum temperatures (medium font)
  M5.dis.setTextSize(2);
  M5.dis.setCursor(0, 60);
  M5.dis.printf("Max: %.1f C\n", maxTemp);
  M5.dis.setCursor(0, 80);
  M5.dis.printf("Min: %.1f C", minTemp);
}

// ---------------------------------------------------------------------------
// Initialise BLE service and characteristic for temperature broadcasting.
// ---------------------------------------------------------------------------
void setupBLE() {
  BLEDevice::init("AtomS3_TempSensor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->setValue("0.0");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

// ---------------------------------------------------------------------------
// Log the temperature to a daily file on the SD card.
// The filename is generated using the current date (YYYY-MM-DD.txt).
// If free space is low, the file is deleted to allow overwriting.
// ---------------------------------------------------------------------------
void logTemperature(float temp) {
  // Generate filename based on date
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = localtime(&epochTime);
  char filename[20];
  sprintf(filename, "/%04d-%02d-%02d.txt", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);

  // Open the file in append mode
  File logFile = SD.open(filename, FILE_APPEND);
  if (logFile) {
    char timeStr[9]; // HH:MM:SS
    sprintf(timeStr, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    logFile.printf("%s,%.1f\n", timeStr, temp);
    logFile.close();
  } else {
    Serial.println("Failed to open log file for writing");
  }

  // Check available space on the SD card (example threshold: 10 MB)
  uint64_t freeBytes = SD.freeBytes();
  if (freeBytes < (10ULL * 1024 * 1024)) {
    SD.remove(filename);
    Serial.println("Low SD space: file overwritten");
  }
}

void setup() {
  // Initialise the AtomS3 and its display
  M5.begin();
  M5.dis.init();
  Serial.begin(115200);

  // Connect to Wi‑Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi‑Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  // Start the NTP client to synchronise time
  timeClient.begin();
  timeClient.update();

  // Initialise the SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialisation failed!");
  } else {
    Serial.println("SD Card initialised.");
  }

  // Start the DS18B20 sensor
  sensors.begin();

  // Set up BLE service for broadcasting temperature
  setupBLE();
}

void loop() {
  M5.update();
  timeClient.update();

  // Reset daily speaker alerts if a new day has started
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = localtime(&epochTime);
  if (ptm->tm_mday != currentDay) {
    currentDay = ptm->tm_mday;
    highTempAlertDone = false;
    lowTempAlertDone = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Request and read the temperature from DS18B20
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.println("Error: Could not read temperature data");
      return;
    }
    currentTemp = tempC;

    // Update daily maximum and minimum temperatures
    if (currentTemp > maxTemp) maxTemp = currentTemp;
    if (currentTemp < minTemp) minTemp = currentTemp;

    // Check for high temperature alert (over 30°C) once per day
    if (currentTemp > 30.0 && !highTempAlertDone) {
      speakMessage("over Temperature");
      highTempAlertDone = true;
    }
    // Check for low temperature alert (below 5°C) once per day
    if (currentTemp < 5.0 && !lowTempAlertDone) {
      speakMessage("Low Temperature, put extra thick undies on");
      lowTempAlertDone = true;
    }

    // Log the temperature reading to the SD card
    logTemperature(currentTemp);

    // Update the BLE characteristic with the current temperature
    char tempStr[10];
    sprintf(tempStr, "%.1f", currentTemp);
    pCharacteristic->setValue(tempStr);
    pCharacteristic->notify();

    // Update the display with the current data
    updateDisplay();

    Serial.printf("Logged Temperature: %.1f C\n", currentTemp);
  }
}
