/*
 * Implementierung der GPS-Funktionen
 */

#include "gps_module.h"
#include <Arduino.h>

// ==============================================
// GLOBALE VARIABLEN
// ==============================================

TinyGPSPlus gpsParser;
GPSData currentGPSData = {false, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0};

// Vorherige GPS-Position für Kursberechnung
static double prevLatitude = 0.0;
static double prevLongitude = 0.0;
static bool hasPrevPosition = false;

// ==============================================
// GPS INITIALISIERUNG
// ==============================================

bool initGPS() {
  setupGPSSerial();
  
  DEBUG_PRINTLN(F("=== GPS INITIALISIERUNG ==="));
  DEBUG_PRINTLN(F("GPS initialisiert. RX an Pin 19, TX an Pin 18"));
  DEBUG_PRINT(F("GPS Baudrate: "));
  DEBUG_PRINTLN(GPS_BAUD);
  DEBUG_PRINTLN(F("Hardware Serial1 konfiguriert"));
  DEBUG_PRINTLN(F("Teste GPS-Kommunikation..."));
  
  // GPS-Test: 3 Sekunden auf Daten warten
  unsigned long testStart = millis();
  int bytesReceived = 0;
  
  while (millis() - testStart < 3000) {
    if (Serial1.available()) {
      char c = Serial1.read();
      bytesReceived++;
      // Erste GPS-Zeichen zur Diagnose ausgeben
      if (bytesReceived <= 50) {
        DEBUG_PRINT(c);
      }
    }
    delay(10);
  }
  
  DEBUG_PRINTLN();
  DEBUG_PRINT(F("GPS Bytes empfangen: "));
  DEBUG_PRINTLN(bytesReceived);
  
  if (bytesReceived > 0) {
    DEBUG_PRINTLN(F("GPS Hardware: OK - Daten empfangen"));
  } else {
    DEBUG_PRINTLN(F("GPS Hardware: FEHLER - Keine Daten"));
    DEBUG_PRINTLN(F("Prüfe Verkabelung:"));
    DEBUG_PRINTLN(F("- GPS VCC → 3.3V oder 5V"));
    DEBUG_PRINTLN(F("- GPS GND → Arduino GND"));
    DEBUG_PRINTLN(F("- GPS TX → Arduino Pin 19 (RX1)"));
    DEBUG_PRINTLN(F("- GPS RX → Arduino Pin 18 (TX1)"));
  }
  
  return bytesReceived > 0;
}

void setupGPSSerial() {
  Serial1.begin(GPS_BAUD);
  // Hardware Serial1 für Arduino Mega:
  // RX1 (Pin 19) - GPS TX Ausgang
  // TX1 (Pin 18) - GPS RX Eingang
}

// ==============================================
// GPS DATENVERARBEITUNG
// ==============================================

void updateGPS() {
  // GPS-Daten über Hardware Serial1 einlesen
  while (Serial1.available() > 0) {
    if (gpsParser.encode(Serial1.read())) {
      // Neue gültige GPS-Daten erhalten
      currentGPSData.lastUpdate = millis();
    }
  }
}

bool readGPSData(GPSData* data) {
  if (!data) return false;
  
  // GPS-Aktivität prüfen
  static unsigned long lastDebugTime = 0;
  static int lastByteCount = 0;
  static int currentByteCount = 0;
  
  // Byte-Zähler für GPS-Aktivität
  while (Serial1.available()) {
    char c = Serial1.read();
    currentByteCount++;
    if (gpsParser.encode(c)) {
      // Neue gültige GPS-Daten erhalten
      DEBUG_PRINTLN(F("*** GPS: Neuer gültiger Datensatz empfangen ***"));
    }
  }
  
  // Debug-Ausgabe alle 5 Sekunden
  if (millis() - lastDebugTime > 5000) {
    int bytesThisPeriod = currentByteCount - lastByteCount;
    DEBUG_PRINT(F("GPS Debug: Bytes/5s = "));
    DEBUG_PRINT(bytesThisPeriod);
    DEBUG_PRINT(F(", Satelliten = "));
    DEBUG_PRINT(gpsParser.satellites.value());
    DEBUG_PRINT(F(", Char-Count = "));
    DEBUG_PRINT(gpsParser.charsProcessed());
    DEBUG_PRINT(F(", Sentences = "));
    DEBUG_PRINT(gpsParser.sentencesWithFix());
    DEBUG_PRINT(F("/"));
    DEBUG_PRINTLN(gpsParser.passedChecksum());
    
    lastDebugTime = millis();
    lastByteCount = currentByteCount;
  }
  
  data->isValid = gpsParser.location.isValid();
  
  if (data->isValid) {
    data->latitude = gpsParser.location.lat();
    data->longitude = gpsParser.location.lng();
    data->altitude = gpsParser.altitude.meters();
    data->speedKmh = gpsParser.speed.kmph();      // Geschwindigkeit in km/h
    
    // Kurs aus Koordinaten-Änderung berechnen (präziser als GPS-Kurs)
    if (hasPrevPosition && data->speedKmh > 1.0) {
      // Nur bei Bewegung > 1 km/h berechnen
      data->course = calculateCourse(prevLatitude, prevLongitude, 
                                   data->latitude, data->longitude);
    } else {
      data->course = 0.0;  // Stillstand oder erste Messung
    }
    
    // Aktuelle Position für nächste Berechnung speichern
    prevLatitude = data->latitude;
    prevLongitude = data->longitude;
    hasPrevPosition = true;
    
    data->satellites = gpsParser.satellites.value();
    data->quality = gpsParser.hdop.value();       // HDOP als Qualitätsindikator
    data->lastUpdate = millis();
  } else {
    data->latitude = 0.0;
    data->longitude = 0.0;
    data->altitude = 0.0;
    data->speedKmh = 0.0;
    data->course = 0.0;
    data->satellites = 0;
    data->quality = 0;
  }
  
  // Aktuelle Daten kopieren
  currentGPSData = *data;
  return data->isValid;
}

void printGPSData(const GPSData* data) {
  if (!data->isValid) {
    DEBUG_PRINTLN(F("GPS: Keine gültigen Daten"));
    return;
  }
  
  DEBUG_PRINT(F("GPS: "));
  Serial.print(data->latitude, 6);
  DEBUG_PRINT(F(", "));
  Serial.print(data->longitude, 6);
  DEBUG_PRINT(F(" | Alt: "));
  Serial.print(data->altitude, 1);
  DEBUG_PRINT(F("m | Speed: "));
  Serial.print(data->speedKmh, 1);
  DEBUG_PRINT(F("km/h | Course: "));
  Serial.print(data->course, 0);
  DEBUG_PRINT(F("° | Sats: "));
  DEBUG_PRINT(data->satellites);
  DEBUG_PRINT(F(" | Q: "));
  DEBUG_PRINTLN(data->quality);
}

// ==============================================
// GPS HILFSFUNKTIONEN
// ==============================================

double calculateCourse(double lat1, double lon1, double lat2, double lon2) {
  // Berechnet den Kurs (Bearing) zwischen zwei GPS-Punkten
  // Rückgabe: Kurs in Grad (0-360°), 0° = Norden
  
  if (lat1 == lat2 && lon1 == lon2) {
    return 0.0; // Keine Bewegung
  }
  
  // Koordinaten in Radiant umwandeln
  double lat1Rad = radians(lat1);
  double lat2Rad = radians(lat2);
  double deltaLonRad = radians(lon2 - lon1);
  
  // Bearing-Formel (Forward Azimuth)
  double y = sin(deltaLonRad) * cos(lat2Rad);
  double x = cos(lat1Rad) * sin(lat2Rad) - sin(lat1Rad) * cos(lat2Rad) * cos(deltaLonRad);
  
  double bearingRad = atan2(y, x);
  
  // Von Radiant zu Grad konvertieren und auf 0-360° normalisieren
  double bearingDeg = degrees(bearingRad);
  if (bearingDeg < 0) {
    bearingDeg += 360.0;
  }
  
  return bearingDeg;
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  // Haversine-Formel für Entfernungsberechnung
  const double R = 6371000; // Erdradius in Metern
  
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  
  double a = sin(dLat/2) * sin(dLat/2) +
             cos(radians(lat1)) * cos(radians(lat2)) *
             sin(dLon/2) * sin(dLon/2);
  
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  return R * c; // Entfernung in Metern
}

void formatCoordinates(double lat, double lon, char* buffer, byte bufferSize) {
  if (lat == 0.0 && lon == 0.0) {
    snprintf(buffer, bufferSize, "--,--");
  } else {
    snprintf(buffer, bufferSize, "%.6f,%.6f", lat, lon);
  }
}

void formatGPSString(const GPSData* data, char* buffer, byte bufferSize) {
  if (!data->isValid) {
    snprintf(buffer, bufferSize, "--,--");
  } else {
    snprintf(buffer, bufferSize, "%.6f,%.6f", data->latitude, data->longitude);
  }
}

// ==============================================
// GPS STATUS
// ==============================================

bool isGPSConnected() {
  return (millis() - currentGPSData.lastUpdate) < 5000; // 5 Sekunden Timeout
}

unsigned long getLastGPSUpdate() {
  return currentGPSData.lastUpdate;
}

byte getGPSQuality() {
  return currentGPSData.quality;
}
