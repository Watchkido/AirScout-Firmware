/*
 * Implementierung des Data-Logger-Moduls
 */

#include "data_logger.h"
#include "gps_module.h"  // Für TinyGPSPlus GPS-Zeit
#include <Arduino.h>

// ==============================================
// GLOBALE VARIABLEN
// ==============================================

char globalLogFilename[FILENAME_LENGTH];
bool sdCardInitialized = false;

// ==============================================
// SD-KARTE TIMESTAMP CALLBACK
// ==============================================

// Callback-Funktion für korrekte Datei-Timestamps
void dateTime(uint16_t* date, uint16_t* time) {
  RTCData currentTime;
  readRTCData(&currentTime);
  
  // FAT Datei-Timestamp Format (MS-DOS kompatibel)
  // Datum: Bits 15-9 = Jahr-1980, Bits 8-5 = Monat, Bits 4-0 = Tag
  *date = ((currentTime.year - 1980) << 9) | (currentTime.month << 5) | currentTime.day;
  
  // Zeit: Bits 15-11 = Stunde, Bits 10-5 = Minute, Bits 4-0 = Sekunde/2
  *time = (currentTime.hour << 11) | (currentTime.minute << 5) | (currentTime.second >> 1);
}

// ==============================================
// SD-KARTE INITIALISIERUNG
// ==============================================

bool initSDCard() {
  DEBUG_PRINT(F("Initialisiere SD-Karte..."));
  
  if (!SD.begin(SD_CHIP_SELECT)) {
    DEBUG_PRINTLN(F("FEHLER: SD-Karte nicht gefunden oder defekt!"));
    sdCardInitialized = false;
    return false;
  }
  
  // Timestamp-Callback für korrekte Datei-Zeiten setzen
  SdFile::dateTimeCallback(dateTime);
  
  DEBUG_PRINTLN(F("OK"));
  sdCardInitialized = true;
  delay(SD_INIT_DELAY);
  
  printSDCardInfo();
  return true;
}

bool checkSDCard() {
  return sdCardInitialized && SD.begin(SD_CHIP_SELECT);
}

void printSDCardInfo() {
  // Grundlegende SD-Karten Informationen ausgeben
  DEBUG_PRINTLN(F("=== SD-Karte Info ==="));
  DEBUG_PRINT(F("Karte erkannt: "));
  DEBUG_PRINTLN(sdCardInitialized ? F("JA") : F("NEIN"));
}

// ==============================================
// DATEI-MANAGEMENT
// ==============================================

bool createLogFile(char* filename, uint8_t filenameSize) {
  if (!sdCardInitialized) {
    DEBUG_PRINTLN(F("FEHLER: SD-Karte nicht initialisiert!"));
    return false;
  }
  
  // Dateiname mit Zeitstempel generieren
  generateFilename(filename, filenameSize);
  
  // Vorhandene Datei löschen (für neuen Start)
  if (SD.exists(filename)) {
    SD.remove(filename);
    DEBUG_PRINT(F("Alte Datei gelöscht: "));
    DEBUG_PRINTLN(filename);
  }
  
  // Neue Datei erstellen und Header schreiben
  File logFile = SD.open(filename, FILE_WRITE);
  if (!logFile) {
    DEBUG_PRINT(F("FEHLER: Kann Datei nicht erstellen: "));
    DEBUG_PRINTLN(filename);
    return false;
  }
  
  // Header schreiben
  RTCData currentTime;
  readRTCData(&currentTime);
  
  logFile.print(F("# Umweltkontrollsystem Log\n"));
  logFile.print(F("# Start: "));
  logFile.print(currentTime.year);
  logFile.print('-');
  if (currentTime.month < 10) logFile.print('0');
  logFile.print(currentTime.month);
  logFile.print('-');
  if (currentTime.day < 10) logFile.print('0');
  logFile.print(currentTime.day);
  logFile.print(' ');
  if (currentTime.hour < 10) logFile.print('0');
  logFile.print(currentTime.hour);
  logFile.print(':');
  if (currentTime.minute < 10) logFile.print('0');
  logFile.print(currentTime.minute);
  logFile.print(':');
  if (currentTime.second < 10) logFile.print('0');
  logFile.println(currentTime.second);
  
  // Sensor-Konfiguration und Pin-Belegung
  logFile.println(F("# ===== SENSOR-KONFIGURATION ====="));
  logFile.println(F("# TEMPERATURSENSOR:"));
  logFile.println(F("#   MQ-GASSENSOR: Aufwärmen 24-48h (erstmalig), 5-10min (täglich), Antwortzeit <10s"));
  logFile.println(F("#   Kalibrierung: R0 in sauberer Luft, monatlich erneuern | Formel: ppm = A*(Rs/R0)^B"));
  logFile.println(F("#   Umgebung: -10 bis +50°C, trocken, zugfrei | Werte eher relativ als absolut nutzen"));
  logFile.println(F("#   DHT11: Pin 22 (Digital) - Temp + Luftfeuchtigkeit, 0.5Hz [Temperature_DHT_C, Humidity_RH]"));
  logFile.println(F("#   DS18B20: Pin 8 (OneWire) - 4.7K Pull-up zu 5V erforderlich"));
  logFile.println(F("# GPS-MODUL:"));
  logFile.println(F("#   RX: Pin 19, TX: Pin 18 (Serial1, 9600 Baud), 200Hz [GPS_Lat, GPS_Lon, GPS_Alt, GPS_Speed, GPS_Course, GPS_Sats, GPS_DateTime]"));
  logFile.println(F("# GAS-SENSOREN (MQ-Serie, 0.5Hz):"));
  logFile.println(F("#   MQ2 (Flüssiggas (LPG), i -Butan(C4H10), Propan (C3H8),Methan (CH4), Wasserstoff (H2), Alkohol, Rauch - 100 bis 10000ppm ): A0 [MQ2]"));
  logFile.println(F("#   MQ3 (Alkohol,Ethanol (C2H5OH) - 100 - 10000 ppm): A1 [MQ3]"));
  logFile.println(F("#   MQ4 (Methan,CNG - 200-10000ppm Erkennung von CH4, Erdgas und LNG. Vermeiden sie Alkohol-, Koch- und Zigarettenrauch.): A2 [MQ4]"));
  logFile.println(F("#   MQ5 (Erdgas,LPG - 200-10000ppm LPG,LNG Natural gas, iso-butane, propane Town gas ): A3 [MQ5]"));
  logFile.println(F("#   MQ6 (Flüssiggas (Liquefied Petrol Gas) wie Butan, Propan, Methan und brennbare Gase - 300 - 10'000 ppm): A4 [MQ6]"));
  logFile.println(F("#   MQ7 (Kohlenmonoxid CO - 10-1000 ppm CO, 100-10000 ppm brennbare Gase): A5 [MQ7]"));
  logFile.println(F("#   MQ8 (Wasserstoff H2 - Wasserstoff (H2), viele wasserstoffhaltige Gase - 100 - 1000 ppm): A6 [MQ8]"));
  logFile.println(F("#   MQ9 (CO,entflammbare Gase - 0–2000 ppm Kohlenmonoxid 500–10.000 ppm CH4 500–10.000 ppm Flüssiggas): A7 [MQ9]"));
  logFile.println(F("#   MQ135 (Luftqualitaet CO2,NH3,NOx - Giftgase wie Benzon, Alkohol, Rauch sowie Verunreinigungen in der Luft. Der MQ-135 misst eine Gaskonzentration von 10 bis 1000ppm und ist ideal zum Erkennen eines Gaslecks, als Gas-Alarm): A8 [MQ135]"));
  logFile.println(F("# MIKROFONE (100kHz Burst):"));
  logFile.println(F("#   Mikrofon Klein: A9, Mikrofon Gross: A10 [Mic1, Mic2]"));
  logFile.println(F("# RADIOAKTIVITAET:"));
  logFile.println(F("#   Geigerzaehler: Pin 29 (Digital), 200Hz Polling [Radiation_CPS]"));
  logFile.println(F("# LICHTSENSOR:"));
  logFile.println(F("#   LDR: Pin A11, 0.5Hz (0-1023 = Dunkel-Hell) [Light_Level, Light_Percent]"));
  logFile.println(F("# ZEIT/TIMESTAMP:"));
  logFile.println(F("#   Sekunden seit Mitternacht + Millisekunden [SecSinceMidnight-MS, DateTime]"));
  logFile.println(F("# SD-KARTE:"));
  logFile.println(F("#   CS: Pin 10, MOSI: 51, MISO: 50, SCK: 52 (SPI)"));
  logFile.println(F("# RTC (Real-Time Clock):"));
  logFile.println(F("#   SDA: Pin 20, SCL: Pin 21 (I2C)"));
  logFile.println(F("# SYSTEM: Arduino Mega 2560, 5V Versorgung"));
  logFile.println(F("# ABTASTRATEN: GPS/Radiation=200Hz, Sensoren=0.5Hz, Mikro=100kHz"));
  logFile.println(F("# ==================================="));
  
  // CSV Header mit Komma-Trennung (GPS-Zeit hinzugefügt)
  logFile.println(F("SecSinceMidnight-MS,Temperature_DHT_C,Humidity_RH,Light_Level,Light_Percent,GPS_Lat,GPS_Lon,GPS_Alt,GPS_Speed,GPS_Course,GPS_Sats,MQ2,MQ3,MQ4,MQ5,MQ6,MQ7,MQ8,MQ9,MQ135,Mic1,Mic2,Radiation_CPS,DateTime,GPS_DateTime"));
  
  logFile.close();
  
  // Globalen Dateinamen setzen
  strncpy(globalLogFilename, filename, FILENAME_LENGTH - 1);
  globalLogFilename[FILENAME_LENGTH - 1] = '\0';
  
  DEBUG_PRINT(F("Log-Datei erstellt: "));
  DEBUG_PRINTLN(filename);
  
  delay(SD_INIT_DELAY);
  return true;
}

void generateFilename(char* filename, uint8_t filenameSize) {
  RTCData currentTime;
  readRTCData(&currentTime);
  
  // 8.3 Format: MMDDhhmm.CSV (Monat, Tag, Stunde, Minute - 8 Zeichen, MESZ Zeit)
  snprintf(filename, filenameSize, "%02d%02d%02d%02d.CSV", 
           currentTime.month, currentTime.day, currentTime.hour, currentTime.minute);
}

// ==============================================
// DATENPROTOKOLLIERUNG
// ==============================================

bool logSensorData(float temperature_dht, float humidity, const GPSData* gps, const RTCData* rtc) {
  if (!sdCardInitialized || strlen(globalLogFilename) == 0) {
    DEBUG_PRINTLN(F("FEHLER: Kein Log-File!"));
    return false;
  }
  
  // Datei zum Anhängen öffnen mit Error-Check
  File logFile = SD.open(globalLogFilename, FILE_WRITE);
  if (!logFile) {
    DEBUG_PRINTLN(F("FEHLER: Log-Datei kann nicht geöffnet werden!"));
    return false;
  }
  
  // Sicherstellen, dass Datei am Ende ist
  logFile.seek(logFile.size());
  
  // STABILER Timestamp: Fortlaufende Sekunden seit Mitternacht + Millisekunden
  static unsigned long logCounter = 0;
  logCounter++;
  
  unsigned long secondsSinceMidnight = (rtc->hour * 3600UL) + (rtc->minute * 60UL) + rtc->second;
  unsigned int ms = millis() % 1000;
  
  char timestamp[INDEX_LENGTH];
  snprintf(timestamp, sizeof(timestamp), "%05lu-%03d", 
           secondsSinceMidnight, ms);
  
  DEBUG_PRINT(F("Log#"));
  DEBUG_PRINT(logCounter);
  DEBUG_PRINT(F(" TS:"));
  DEBUG_PRINTLN(timestamp);
  
  // Gas-Sensoren lesen (lokal begrenzt)
  int gasSensors[MAX_GAS_SENSORS];
  readAllGasSensors(gasSensors);
  
  // Mikrofone lesen (lokal begrenzt)  
  int microphones[MAX_MICROPHONES];
  readAllMicrophones(microphones);
  
  // Radioaktivität (Klicks pro Sekunde für CSV)
  int radiationCount = getRadiationClicksPerSecond();
  
  // Kompakte CSV-Zeile in einem Zug schreiben 
  String csvLine = "";
  csvLine += timestamp;
  csvLine += ",";
  csvLine += String(temperature_dht, 1);      // DHT11 Temperatur
  csvLine += ",";
  csvLine += String(humidity, 1);             // DHT11 Luftfeuchtigkeit
  csvLine += ",";
  
  // Lichtsensor-Daten
  int lightLevel = readLightSensor();
  float lightPercent = getLightPercent();
  csvLine += String(lightLevel);              // Licht-Level (0-1023)
  csvLine += ",";
  csvLine += String(lightPercent, 1);         // Licht-Prozent (0-100%)
  csvLine += ",";
  
  // GPS-Daten mit Komma-Trennung
  if (gps->isValid) {
    csvLine += String(gps->latitude, 4);     // Breitengrad
    csvLine += ",";
    csvLine += String(gps->longitude, 4);    // Längengrad
    csvLine += ",";
    csvLine += String(gps->altitude, 1);     // Höhe in m
    csvLine += ",";
    csvLine += String(gps->speedKmh, 1);     // Geschwindigkeit in km/h
    csvLine += ",";
    csvLine += String(gps->course, 0);       // Richtung in Grad
    csvLine += ",";
    csvLine += String(gps->satellites);      // Satelliten
  } else {
    csvLine += "--,--,--,--,--,0";
  }
  csvLine += ",";
  
  // Gas-Sensoren mit Komma-Trennung (alle 9 Sensoren einzeln)
  for (uint8_t i = 0; i < MAX_GAS_SENSORS; i++) {
    csvLine += String(gasSensors[i]);
    csvLine += ",";
  }
  
  // Mikrofone mit Komma-Trennung
  csvLine += String(microphones[0]);
  csvLine += ",";
  csvLine += String(microphones[1]);
  csvLine += ",";
  
  // Radioaktivität
  csvLine += String(radiationCount);
  csvLine += ",";
  
  // Maschinenlesbares Zeitformat mit MEZ/MESZ
  if (rtc->year > 2000) {
    char dateTimeStr[40];
    formatLocalDateTime(rtc, dateTimeStr, sizeof(dateTimeStr));
    csvLine += dateTimeStr;
  } else {
    csvLine += "----/--/-- --:--:-- MEZ";
  }
  csvLine += ",";
  
  // GPS-Zeit als letzte Spalte hinzufügen
  extern TinyGPSPlus gpsParser;  // GPS-Parser aus gps_module.cpp
  if (gps->isValid && gpsParser.date.isValid() && gpsParser.time.isValid() && gpsParser.date.year() > 2000) {
    char gpsDateTime[30];
    snprintf(gpsDateTime, sizeof(gpsDateTime), "%04d-%02d-%02d %02d:%02d:%02d UTC",
             gpsParser.date.year(), gpsParser.date.month(), gpsParser.date.day(), 
             gpsParser.time.hour(), gpsParser.time.minute(), gpsParser.time.second());
    csvLine += gpsDateTime;
  } else {
    csvLine += "----/--/-- --:--:-- UTC";
  }
  
  // Komplette Zeile in einem Schreibvorgang
  logFile.println(csvLine);
  
  // WICHTIG: Sofort synchronisieren und schließen
  logFile.flush();
  logFile.close();
  
  // Serial-Ausgabe: Exakt das gleiche wie auf SD-Karte geschrieben
  DEBUG_PRINT(F("CSV: "));
  DEBUG_PRINTLN(csvLine);  // Zeigt die komplette CSV-Zeile an
  
  return true;
}

// ==============================================
// HILFSFUNKTIONEN
// ==============================================

bool isSDCardAvailable() {
  return sdCardInitialized;
}
