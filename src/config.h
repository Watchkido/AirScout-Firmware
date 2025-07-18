/*
 * Konfigurationsdatei für das Umweltkontrollsystem
 * Arduino Mega 2560 Projekt
 * #define DEBUG_ENABLED 1          // ✅ EINGESCHALTET für Sensor-Tests!
#define USE_FLASH_STRINGS 1      // F() Makro für Strings verwenden
#define ENABLE_DETAILED_LOGGING 0 // Detaillierte Logs ausgeschaltet* Zentrale Konfiguration aller Hardware-Parameter und Konstanten
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <stdint.h>

// ==============================================
// HARDWARE KONFIGURATION
// ==============================================

// Pin-Definitionen
// const uint8_t TEMP_SENSOR_PIN = 8;        // OneWire Temperatursensor (DEAKTIVIERT)
const uint8_t DHT_SENSOR_PIN = 22;        // DHT11 Temperatur & Luftfeuchtigkeit
const uint8_t SD_CHIP_SELECT = 10;        // SD-Karte CS Pin
const uint8_t RADIATION_INPUT_PIN = 29;   // Geigerzähler/Radioaktivitätssensor

// OLED Display (I2C)
const uint8_t OLED_SCREEN_WIDTH = 128;    // OLED Display Breite in Pixel
const uint8_t OLED_SCREEN_HEIGHT = 64;    // OLED Display Höhe in Pixel
const int8_t OLED_RESET_PIN = -1;         // Reset Pin (-1 = shared Arduino reset pin)
const uint8_t OLED_I2C_ADDRESS = 0x3C;    // Standard I2C Adresse für 128x64 OLED

// GPS Hardware Serial (Arduino Mega)
// RX1 (Pin 19) - GPS TX, TX1 (Pin 18) - GPS RX
const uint32_t GPS_BAUD = 9600;
const uint32_t SERIAL_BAUD = 9600;

// Gas-Sensoren (MQ-Serie) - Analoge Pins
const uint8_t MQ2_PIN = A0;   // Methan, Butan, LPG, Rauch
const uint8_t MQ3_PIN = A1;   // Alkohol, Ethanol
const uint8_t MQ4_PIN = A2;   // Methan, CNG Gas
const uint8_t MQ5_PIN = A3;   // Natürliche Gase, LPG
const uint8_t MQ6_PIN = A4;   // LPG, Butangas
const uint8_t MQ7_PIN = A5;   // Kohlenmonoxid (CO)
const uint8_t MQ8_PIN = A6;   // Wasserstoff (H2)
const uint8_t MQ9_PIN = A7;   // CO, entflammbare Gase
const uint8_t MQ135_PIN = A8; // Luftqualität (CO2, NH3, NOx)
const uint8_t MIC_KLEIN_PIN = A9;
const uint8_t MIC_GROSS_PIN = A10;
const uint8_t LDR_PIN = A11;  // Lichtsensor (Light Dependent Resistor)

// ==============================================
// TIMING KONFIGURATION
// ==============================================

const unsigned long SENSOR_INTERVAL = 2000;    // Gas-Sensoren Intervall (ms)
const unsigned long LOGGING_INTERVAL = 2000;   // Hauptloop Logging Intervall (2 Sekunden für stabilere SD-Karte)
const unsigned long TEMP_SENSOR_DELAY = 1000;  // Temperatur-Konvertierungszeit (ms)
const unsigned long SD_INIT_DELAY = 200;       // SD-Karte Initialisierung (ms)
const unsigned long OLED_UPDATE_INTERVAL = 2000; // OLED Display Update alle 2 Sekunden

// ==============================================
// SPEICHER-OPTIMIERUNG
// ==============================================

const uint8_t FILENAME_LENGTH = 16;       // Reduziert von 32 auf 16
const uint8_t INDEX_LENGTH = 20;          // Reduziert von 32 auf 20  
const uint8_t TEMP_BUFFER_SIZE = 12;
const uint8_t ADDR_BUFFER_SIZE = 8;

// Zusätzliche Speicher-Konstanten
const uint8_t MAX_GAS_SENSORS = 9;
const uint8_t MAX_MICROPHONES = 2;
const uint8_t CSV_BUFFER_SIZE = 128;       // Für CSV-Zeilen

// ==============================================
// GPS REFERENZPUNKT
// ==============================================

const double HOME_LAT = 49.352622;  // QTH Breitengrad
const double HOME_LON = 8.165440;   // QTH Längengrad
const float GPS_INVALID_F = 1000000.0;

// ==============================================
// DATEISYSTEM
// ==============================================

const uint8_t MAX_FILENAME_LEN = 13;  // 8.3 Format für FAT16/32

// ==============================================
// SENSOR KALIBRIERUNG
// ==============================================

const unsigned long GAS_SENSOR_WARMUP = 10000;  // Gas-Sensoren Aufwärmzeit (ms)
const float TEMP_PRECISION = 0.0625;        

// ==============================================
// DEBUGGING
// ==============================================

#define DEBUG_ENABLED 1          // � EINSCHALTEN für Debugging!
#define USE_FLASH_STRINGS 1      // F() Makro für Strings verwenden
#define ENABLE_DETAILED_LOGGING 1 // Detaillierte Logs einschalten

#if DEBUG_ENABLED
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)           // Leer = kein Code generiert
  #define DEBUG_PRINTLN(x)         // Leer = kein Code generiert
#endif

// Memory-kritische Warnungen
#define RAM_WARNING_THRESHOLD 512    // Warnung bei < 512 Bytes
#define RAM_CRITICAL_THRESHOLD 256   // Kritisch bei < 256 Bytes

#endif // CONFIG_H
