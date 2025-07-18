/*
 * Implementierung des RTC-Moduls
 */

#include "rtc_module.h"
#include "gps_module.h"
#include <Arduino.h>

// ==============================================
// GLOBALE VARIABLEN
// ==============================================

RTC_DS1307 rtcClock;

// ==============================================
// RTC INITIALISIERUNG
// ==============================================

bool initRTC() {
  DEBUG_PRINTLN(F("Initialisiere RTC..."));
  
  Wire.begin();
  rtcClock.begin();
  
  if (!rtcClock.isrunning()) {
    DEBUG_PRINTLN(F("WARNUNG: RTC läuft nicht! Setze Compile-Zeit..."));
    setRTCFromCompileTime();
    return false;
  }
  
  DEBUG_PRINTLN(F("RTC erfolgreich initialisiert."));
  printCurrentTime();
  return true;
}

bool isRTCRunning() {
  return rtcClock.isrunning();
}

void setRTCTime(int year, byte month, byte day, byte hour, byte minute, byte second) {
  DateTime newTime(year, month, day, hour, minute, second);
  rtcClock.adjust(newTime);
  
  DEBUG_PRINT(F("RTC Zeit gesetzt: "));
  DEBUG_PRINT(year);
  DEBUG_PRINT('-');
  DEBUG_PRINT(month);
  DEBUG_PRINT('-');
  DEBUG_PRINT(day);
  DEBUG_PRINT(' ');
  DEBUG_PRINT(hour);
  DEBUG_PRINT(':');
  DEBUG_PRINT(minute);
  DEBUG_PRINT(':');
  DEBUG_PRINTLN(second);
}

void setRTCFromCompileTime() {
  // Setzt RTC auf Compile-Zeit des Sketches
  rtcClock.adjust(DateTime(F(__DATE__), F(__TIME__)));
  DEBUG_PRINTLN(F("RTC auf Compile-Zeit gesetzt."));
}

// ==============================================
// ZEIT LESEN/SCHREIBEN
// ==============================================

bool readRTCData(RTCData* data) {
  if (!data) return false;
  
  DateTime now = rtcClock.now();
  
  data->year = now.year();
  data->month = now.month();
  data->day = now.day();
  data->hour = now.hour();
  data->minute = now.minute();
  data->second = now.second();
  data->timestamp = now.unixtime();
  
  // Erweiterte Validierung: RTC läuft UND Zeit ist plausibel
  bool rtcRunning = isRTCRunning();
  bool timeValid = (data->year >= 2020 && data->year <= 2030 && 
                   data->hour >= 0 && data->hour <= 23 &&
                   data->minute >= 0 && data->minute <= 59);
  
  data->isValid = rtcRunning && timeValid;
  
  DEBUG_PRINT(F("RTC: Running="));
  DEBUG_PRINT(rtcRunning);
  DEBUG_PRINT(F(", TimeValid="));
  DEBUG_PRINT(timeValid);
  DEBUG_PRINT(F(", Year="));
  DEBUG_PRINTLN(data->year);
  
  return data->isValid;
}

void printRTCData(const RTCData* data) {
  if (!data->isValid) {
    DEBUG_PRINTLN(F("RTC: Ungültige Zeit"));
    return;
  }
  
  DEBUG_PRINT(F("Zeit: "));
  if (data->day < 10) DEBUG_PRINT('0');
  DEBUG_PRINT(data->day);
  DEBUG_PRINT('.');
  if (data->month < 10) DEBUG_PRINT('0');
  DEBUG_PRINT(data->month);
  DEBUG_PRINT('.');
  DEBUG_PRINT(data->year);
  DEBUG_PRINT(F(" - "));
  if (data->hour < 10) DEBUG_PRINT('0');
  DEBUG_PRINT(data->hour);
  DEBUG_PRINT(':');
  if (data->minute < 10) DEBUG_PRINT('0');
  DEBUG_PRINT(data->minute);
  DEBUG_PRINT(':');
  if (data->second < 10) DEBUG_PRINT('0');
  DEBUG_PRINTLN(data->second);
}

void printCurrentTime() {
  RTCData currentTime;
  if (readRTCData(&currentTime)) {
    printRTCData(&currentTime);
  } else {
    DEBUG_PRINTLN(F("FEHLER: Kann aktuelle Zeit nicht lesen!"));
  }
}

// ==============================================
// ZEIT-HILFSFUNKTIONEN
// ==============================================

void formatTimeString(const RTCData* data, char* buffer, byte bufferSize) {
  if (!data->isValid) {
    snprintf(buffer, bufferSize, "--:--:--");
    return;
  }
  
  snprintf(buffer, bufferSize, "%02d:%02d:%02d", 
           data->hour, data->minute, data->second);
}

void formatDateString(const RTCData* data, char* buffer, byte bufferSize) {
  if (!data->isValid) {
    snprintf(buffer, bufferSize, "--.--.----");
    return;
  }
  
  snprintf(buffer, bufferSize, "%02d.%02d.%04d", 
           data->day, data->month, data->year);
}

void formatTimestamp(const RTCData* data, char* buffer, byte bufferSize) {
  if (!data->isValid) {
    snprintf(buffer, bufferSize, "----/--/-- --:--:--");
    return;
  }
  
  snprintf(buffer, bufferSize, "%04d/%02d/%02d %02d:%02d:%02d", 
           data->year, data->month, data->day,
           data->hour, data->minute, data->second);
}

// ==============================================
// TIMESTAMP-FUNKTIONEN
// ==============================================

unsigned long getRTCTimestamp() {
  DateTime now = rtcClock.now();
  return now.unixtime();
}

bool isTimeValid(const RTCData* data) {
  return data->isValid && 
         data->year >= 2020 && data->year <= 2099 &&
         data->month >= 1 && data->month <= 12 &&
         data->day >= 1 && data->day <= 31 &&
         data->hour <= 23 && data->minute <= 59 && data->second <= 59;
}

// ==============================================
// GPS-SYNCHRONISATION
// ==============================================

bool setRTCFromGPS(int year, int month, int day, int hour, int minute, int second) {
  // UTC zu lokaler Zeit (optional - hier ist UTC)
  DateTime gpsTime(year, month, day, hour, minute, second);
  rtcClock.adjust(gpsTime);
  
  DEBUG_PRINT(F("RTC mit GPS synchronisiert: "));
  DEBUG_PRINT(day);
  DEBUG_PRINT('.');
  DEBUG_PRINT(month);
  DEBUG_PRINT('.');
  DEBUG_PRINT(year);
  DEBUG_PRINT(' ');
  DEBUG_PRINT(hour);
  DEBUG_PRINT(':');
  DEBUG_PRINT(minute);
  DEBUG_PRINT(':');
  DEBUG_PRINT(second);
  DEBUG_PRINTLN(F(" UTC"));
  
  return true;
}

bool syncRTCWithGPS() {
  // Benötigt gps_module.h Include (wird in der Hauptdatei gemacht)
  #ifdef GPS_MODULE_H
    // GPS-Zeit abrufen über TinyGPSPlus
    extern TinyGPSPlus gpsParser;
    
    if (gpsParser.date.isValid() && gpsParser.time.isValid()) {
      int year = gpsParser.date.year();
      int month = gpsParser.date.month();
      int day = gpsParser.date.day();
      int hour = gpsParser.time.hour();
      int minute = gpsParser.time.minute();
      int second = gpsParser.time.second();
      
      // Plausibilitätsprüfung
      if (year >= 2020 && year <= 2099 && month >= 1 && month <= 12) {
        return setRTCFromGPS(year, month, day, hour, minute, second);
      } else {
        DEBUG_PRINTLN(F("GPS-Zeit ungültig, Synchronisation übersprungen"));
        return false;
      }
    } else {
      DEBUG_PRINTLN(F("GPS-Zeit nicht verfügbar"));
      return false;
    }
  #else
    DEBUG_PRINTLN(F("GPS-Modul nicht verfügbar"));
    return false;
  #endif
}

// ==============================================
// ZEITZONE-FUNKTIONEN (MEZ/MESZ)
// ==============================================

bool isDST(int year, int month, int day, int hour) {
  // Vereinfachte MESZ-Berechnung für Europa
  // MESZ: letzter Sonntag März 02:00 bis letzter Sonntag Oktober 03:00
  
  if (month < 3 || month > 10) return false; // Jan, Feb, Nov, Dez = MEZ
  if (month > 3 && month < 10) return true;  // Apr-Sep = MESZ
  
  // März und Oktober: genauer berechnen
  int lastSunday;
  if (month == 3) {
    // Letzter Sonntag im März
    lastSunday = 31 - ((5 * year / 4 + 4) % 7);
    return (day > lastSunday || (day == lastSunday && hour >= 2));
  } else { // Oktober
    // Letzter Sonntag im Oktober
    lastSunday = 31 - ((5 * year / 4 + 1) % 7);
    return (day < lastSunday || (day == lastSunday && hour < 3));
  }
}

void adjustUTCToLocal(RTCData* data) {
  // Konvertiert UTC zu lokaler Zeit (MEZ/MESZ)
  if (!data || !data->isValid) return;
  
  int offset = isDST(data->year, data->month, data->day, data->hour) ? 2 : 1; // MESZ = +2, MEZ = +1
  
  data->hour += offset;
  if (data->hour >= 24) {
    data->hour -= 24;
    data->day++;
    // Vereinfachte Datumsberechnung (für die meisten Fälle ausreichend)
    if (data->day > 31) {
      data->day = 1;
      data->month++;
      if (data->month > 12) {
        data->month = 1;
        data->year++;
      }
    }
  }
}

void formatLocalDateTime(const RTCData* data, char* buffer, int bufferSize) {
  if (!data->isValid) {
    snprintf(buffer, bufferSize, "----/--/-- --:--:--");
    return;
  }
  
  RTCData localTime = *data;
  adjustUTCToLocal(&localTime);
  
  const char* timezone = isDST(data->year, data->month, data->day, data->hour) ? "MESZ" : "MEZ";
  
  snprintf(buffer, bufferSize, "%04d-%02d-%02d %02d:%02d:%02d %s",
           localTime.year, localTime.month, localTime.day,
           localTime.hour, localTime.minute, localTime.second, timezone);
}
