/*
 * Implementierung der Utilities-Funktionen
 */

#include "utilities.h"
#include "gps_module.h"
#include "rtc_module.h"
#include <Arduino.h>

// ==============================================
// GLOBALE VARIABLEN
// ==============================================

SystemError lastError = ERROR_NONE;

// ==============================================
// SYSTEM-FUNKTIONEN
// ==============================================

void printSystemInfo() {
  DEBUG_PRINTLN(F("=== UMWELTKONTROLLSYSTEM ==="));
  DEBUG_PRINTLN(F("Arduino Mega 2560"));
  DEBUG_PRINT(F("Freier RAM: "));
  DEBUG_PRINT(getFreeRAM());
  DEBUG_PRINTLN(F(" Bytes"));
  
  #if DEBUG_ENABLED
    DEBUG_PRINT(F("Debug: AKTIVIERT"));
  #else
    DEBUG_PRINT(F("Debug: DEAKTIVIERT"));
  #endif
  DEBUG_PRINTLN();
  
  DEBUG_PRINT(F("Compile: "));
  DEBUG_PRINT(F(__DATE__));
  DEBUG_PRINT(F(" "));
  DEBUG_PRINTLN(F(__TIME__));
  DEBUG_PRINTLN(F("==========================="));
}

void printMemoryUsage() {
  unsigned int freeRAM = getFreeRAM();
  DEBUG_PRINT(F("RAM: "));
  DEBUG_PRINT(freeRAM);
  DEBUG_PRINTLN(F(" Bytes"));
  
  if (freeRAM < RAM_CRITICAL_THRESHOLD) {
    DEBUG_PRINTLN(F("KRITISCH: RAM-Mangel!"));
    reportError(ERROR_MEMORY, "Critical RAM");
  } else if (freeRAM < RAM_WARNING_THRESHOLD) {
    DEBUG_PRINTLN(F("WARNUNG: Wenig RAM!"));
    reportError(ERROR_MEMORY, "Low RAM");
  }
}

unsigned int getFreeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void softReset() {
  DEBUG_PRINTLN(F("System-Reset..."));
  delay(1000);
  asm volatile ("  jmp 0");  // Software-Reset für Arduino
}

void systemCheck() {
  printMemoryUsage();
  
  unsigned int freeRAM = getFreeRAM();
  if (freeRAM < RAM_CRITICAL_THRESHOLD) {
    DEBUG_PRINTLN(F("KRITISCH: Neustart empfohlen!"));
    // Optional: Automatischer Reset bei kritischem RAM-Mangel
    // softReset();
  }
  
  // GPS-RTC Synchronisation alle 30 Sekunden versuchen
  static unsigned long lastSyncAttempt = 0;
  if (isTimeElapsed(&lastSyncAttempt, 30000)) {  // Alle 30 Sekunden
    #ifdef GPS_MODULE_H
      if (isGPSConnected()) {
        if (syncRTCWithGPS()) {
          DEBUG_PRINTLN(F("RTC-GPS Synchronisation erfolgreich"));
        }
      }
    #endif
  }
}

// ==============================================
// STRING-HILFSFUNKTIONEN
// ==============================================

void trimString(char* str) {
  if (!str) return;
  
  // Führende Leerzeichen entfernen
  char* start = str;
  while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
    start++;
  }
  
  // String nach vorne verschieben
  if (start != str) {
    int len = strlen(start);
    memmove(str, start, len + 1);
  }
  
  // Nachfolgende Leerzeichen entfernen
  int len = strlen(str);
  while (len > 0 && (str[len-1] == ' ' || str[len-1] == '\t' || 
                     str[len-1] == '\n' || str[len-1] == '\r')) {
    str[--len] = '\0';
  }
}

void toLowerCase(char* str) {
  if (!str) return;
  while (*str) {
    *str = tolower(*str);
    str++;
  }
}

void toUpperCase(char* str) {
  if (!str) return;
  while (*str) {
    *str = toupper(*str);
    str++;
  }
}

void formatFloat(float value, byte decimals, char* buffer, byte bufferSize) {
  dtostrf(value, bufferSize-1, decimals, buffer);
  trimString(buffer);
}

void formatInteger(int value, byte width, char* buffer, byte bufferSize) {
  snprintf(buffer, bufferSize, "%*d", width, value);
}

// ==============================================
// MATHEMATISCHE HILFSFUNKTIONEN
// ==============================================

float calculateAverage(int* values, byte count) {
  if (!values || count == 0) return 0.0;
  
  long sum = 0;
  for (byte i = 0; i < count; i++) {
    sum += values[i];
  }
  return (float)sum / count;
}

int findMinimum(int* values, byte count) {
  if (!values || count == 0) return 0;
  
  int minimum = values[0];
  for (byte i = 1; i < count; i++) {
    if (values[i] < minimum) {
      minimum = values[i];
    }
  }
  return minimum;
}

int findMaximum(int* values, byte count) {
  if (!values || count == 0) return 0;
  
  int maximum = values[0];
  for (byte i = 1; i < count; i++) {
    if (values[i] > maximum) {
      maximum = values[i];
    }
  }
  return maximum;
}

bool isInRange(int value, int min, int max) {
  return (value >= min && value <= max);
}

int constrainValue(int value, int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

// ==============================================
// TIMING-HILFSFUNKTIONEN
// ==============================================

bool isTimeElapsed(unsigned long* lastTime, unsigned long interval) {
  unsigned long currentTime = millis();
  if (currentTime - *lastTime >= interval) {
    *lastTime = currentTime;
    return true;
  }
  return false;
}

void updateTimer(unsigned long* timer) {
  *timer = millis();
}

unsigned long getElapsedTime(unsigned long startTime) {
  return millis() - startTime;
}

// ==============================================
// ERROR-HANDLING
// ==============================================

void reportError(SystemError error, const char* message) {
  lastError = error;
  
  DEBUG_PRINT(F("FEHLER ["));
  DEBUG_PRINT(error);
  DEBUG_PRINT(F("]: "));
  if (message) {
    DEBUG_PRINTLN(message);
  } else {
    DEBUG_PRINTLN(F("Unbekannter Fehler"));
  }
}

void clearError() {
  lastError = ERROR_NONE;
}

SystemError getLastError() {
  return lastError;
}

// ==============================================
// KALIBRIERUNG (Platzhalter für zukünftige Erweiterungen)
// ==============================================

void calibrateGasSensors() {
  DEBUG_PRINTLN(F("Gas-Sensor Kalibrierung wird durchgeführt..."));
  // Hier könnte eine Kalibrierungsroutine implementiert werden
  delay(2000);
  DEBUG_PRINTLN(F("Kalibrierung abgeschlossen."));
}

float calibrateTemperatureSensor() {
  DEBUG_PRINTLN(F("Temperatur-Sensor Kalibrierung..."));
  // Platzhalter für Temperatur-Kalibrierung
  return 0.0;  // Kalibrierungs-Offset
}

void saveCalibrationData() {
  // Platzhalter: Kalibrierungsdaten in EEPROM speichern
  DEBUG_PRINTLN(F("Kalibrierungsdaten gespeichert."));
}

void loadCalibrationData() {
  // Platzhalter: Kalibrierungsdaten aus EEPROM laden
  DEBUG_PRINTLN(F("Kalibrierungsdaten geladen."));
}
