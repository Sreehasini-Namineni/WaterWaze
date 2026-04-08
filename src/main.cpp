#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

const float ADS_GAIN   = 0.1875;   // mV per bit at GAIN_TWOTHIRDS
const float V_REF      = 3.3;
const int   TEMP_PIN   = 46;       // Temperature sensor on GPIO46
const int   ADC_RES    = 4095;     // ESP32-S3 ADC is 12-bit

float convertEC(float voltage) {
  // Placeholder — replace with your probe's formula after calibration
  float ec = voltage * 1000.0;
  return ec;
}

float convertTurbidity(float voltage) {
  // Placeholder — replace with your sensor's formula
  float ntu = voltage * 3000.0 / V_REF;
  return ntu;
}

float convertTemperature(float voltage) {
  // Placeholder — depends on your sensor model, adjust accordingly
  float tempC = (voltage / V_REF) * 100.0;
  return tempC;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  // Set ADC resolution for ESP32-S3
  analogReadResolution(12);

  // Start I2C on Heltec V3 pins
  Wire.begin(41, 42);

  if (!ads.begin(0x48, &Wire)) {
    Serial.println("ERROR: ADS1115 not found! Check wiring.");
    while (1);
  }

  ads.setGain(GAIN_TWOTHIRDS);
  Serial.println("ADS1115 initialized.");
}

void loop() {
  // --- ADS1115 readings ---
  int16_t rawEC   = ads.readADC_SingleEnded(0); // A0
  int16_t rawTurb = ads.readADC_SingleEnded(1); // A1

  float voltEC   = rawEC   * ADS_GAIN / 1000.0;
  float voltTurb = rawTurb * ADS_GAIN / 1000.0;

  // --- ESP32 onboard ADC for temperature ---
  int rawTemp    = analogRead(TEMP_PIN);
  float voltTemp = (rawTemp / (float)ADC_RES) * V_REF;

  // --- Convert to physical values ---
  float ec          = convertEC(voltEC);
  float turbidity   = convertTurbidity(voltTurb);
  float temperature = convertTemperature(voltTemp);

  // --- Print to Serial Monitor ---
  Serial.println("--- Sensor Readings ---");
  Serial.print("EC:          "); Serial.print(voltEC);       Serial.print("V  ->  "); Serial.print(ec);          Serial.println(" mS/cm");
  Serial.print("Turbidity:   "); Serial.print(voltTurb);     Serial.print("V  ->  "); Serial.print(turbidity);   Serial.println(" NTU");
  Serial.print("Temperature: "); Serial.print(voltTemp);     Serial.print("V  ->  "); Serial.print(temperature); Serial.println(" °C");
  Serial.println();

  delay(2000);
}