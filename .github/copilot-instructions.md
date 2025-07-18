# AI Coding Agent Instructions für Umweltkontrollsystem

## Projektübersicht
Dies ist ein Arduino Mega 2560-basiertes **Umweltkontrollsystem** mit modularer Architektur für die Erfassung von Umweltdaten. Das System sammelt GPS-Koordinaten, Gaswerte, Radioaktivitätsdaten, Temperatur/Luftfeuchtigkeit und protokolliert alles auf SD-Karte.

## Projektstruktur & Architektur

### Verzeichnisstruktur
```
001_Umweltkontrollsystem/
├── PlatformIO_Project/              # Hauptprojekt (AKTIV)
│   ├── platformio.ini               # PlatformIO Konfiguration
│   └── src/                         # Quellcode
│       ├── EnvironmentalMonitor.ino # Hauptprogramm
│       ├── config.h                 # Zentrale Konfiguration
│       ├── sensors.{h,cpp}          # Sensor-Management
│       ├── gps_module.{h,cpp}       # GPS-Funktionalität
│       ├── rtc_module.{h,cpp}       # Echtzeituhr
│       ├── data_logger.{h,cpp}      # SD-Karte Datenprotokollierung
│       ├── display.{h,cpp}          # OLED Display
│       └── utilities.{h,cpp}        # Hilfsfunktionen
├── main/                            # Legacy Code (NICHT VERWENDEN)
└── docs/                            # Dokumentation
```

**WICHTIG:** Arbeite nur im `PlatformIO_Project/` Verzeichnis! Die `main/` und `legacy/` Ordner enthalten veralteten Code.

### Modulare Architektur
Das System verwendet **strikte Modularität** mit separaten Header/Source-Dateien:

- **config.h**: Zentrale Pin-Definitionen, Konstanten, Debug-Flags
- **sensors.{h,cpp}**: Alle Sensoren (DHT11, MQ-Serie Gas, Radioaktivität)
- **gps_module.{h,cpp}**: GPS-Positionierung mit TinyGPSPlus
- **rtc_module.{h,cpp}**: Echtzeituhr für Zeitstempel
- **data_logger.{h,cpp}**: CSV-Protokollierung auf SD-Karte
- **display.{h,cpp}**: OLED Display (128x64, I2C)
- **utilities.{h,cpp}**: Speicherverwaltung, allgemeine Funktionen

## Technische Architektur-Prinzipien

### 1. NON-BLOCKING ARCHITEKTUR
**KRITISCH:** Das System verwendet **millis()-basierte non-blocking Architektur**. Verwende NIEMALS `delay()`!

```cpp
// ✅ RICHTIG - Non-blocking
unsigned long lastCheck = 0;
if (millis() - lastCheck >= INTERVAL) {
    lastCheck = millis();
    // Aufgabe ausführen
}

// ❌ FALSCH - Blockierend
delay(1000);  // Stoppt GPS und Radioaktivitätssensor!
```

### 2. STATE MACHINE SENSOR-INITIALISIERUNG
Sensoren werden mit einer State Machine initialisiert:

```cpp
enum SensorInitState {
  SENSOR_INIT_IDLE,
  SENSOR_INIT_DHT_WARMING,
  SENSOR_INIT_GAS_WARMUP,
  SENSOR_INIT_COMPLETE
};
```

### 3. SPEICHER-OPTIMIERUNG
Arduino Mega hat begrenzte Ressourcen. Verwende F() Makros und optimierte Puffergrößen:

```cpp
DEBUG_PRINTLN(F("Text in Flash speichern"));  // ✅ Gut
const uint8_t CSV_BUFFER_SIZE = 128;          // ✅ Optimiert
```

## Hardware-Konfiguration

### Pin-Zuweisungen (config.h)
```cpp
// Wichtige Pins
const uint8_t DHT_SENSOR_PIN = 22;        // DHT11 Temperatur/Luftfeuchtigkeit
const uint8_t SD_CHIP_SELECT = 10;        // SD-Karte
const uint8_t RADIATION_INPUT_PIN = 29;   // Geigerzähler
const uint8_t MQ2_PIN = A0;               // Gas-Sensoren A0-A8
const uint32_t GPS_BAUD = 9600;           // GPS auf Serial1 (Pins 18/19)
```

### Sensor-Hardware
- **DHT11**: Temperatur/Luftfeuchtigkeit auf Pin 22
- **MQ-Serie Gas**: 9 Sensoren auf A0-A8 (Methan, CO, Alkohol, etc.)
- **Radioaktivität**: Digital-Pin 29 mit CPS-Berechnung
- **GPS**: NEO-6M auf Serial1 (19=RX, 18=TX)
- **SD-Karte**: SPI mit CS auf Pin 10
- **OLED**: 128x64 I2C Display (0x3C)

## Programmierung-Patterns

### Radioaktivitätssensor (CPS-Berechnung)
```cpp
// Zeigt Clicks Per Second, NICHT kumulativ
void checkRadiationSensor() {
    static unsigned long lastSecond = 0;
    static int clicksThisSecond = 0;
    
    if (millis() - lastSecond >= 1000) {
        // CPS ausgeben, dann zurücksetzen
        radiationCPS = clicksThisSecond;
        clicksThisSecond = 0;
        lastSecond = millis();
    }
}
```

### GPS Non-Blocking
```cpp
void updateGPS() {
    while (Serial1.available() > 0) {
        if (gps.encode(Serial1.read())) {
            // GPS-Daten verarbeiten
        }
    }
}
```

### SD-Karte CSV-Logging
```cpp
// CSV-Format: Timestamp,Temp,Humidity,GPS_Lat,GPS_Lon,Radiation_CPS,MQ2,MQ3...
String csvLine = createCSVLine(temperature, humidity, gpsLat, gpsLon, 
                               radiationCPS, gasSensorValues);
```

## PlatformIO-Spezifika

### Bibliotheken (platformio.ini)
```ini
lib_deps = 
    adafruit/RTClib@^2.1.1
    mikalhart/TinyGPSPlus@^1.0.3
    adafruit/DHT sensor library@^1.4.4
    adafruit/Adafruit SSD1306@^2.5.7
    arduino-libraries/SD@^1.2.4
```

### Build-Konfiguration
```ini
upload_port = COM8        # Arduino Mega auf COM8
monitor_port = COM8
build_flags = -Os         # Speicher-Optimierung
```

## Debugging & Entwicklung

### Debug-System
```cpp
#define DEBUG_ENABLED 1   // In config.h aktivieren
DEBUG_PRINTLN(F("Debug message"));  // Nur wenn DEBUG_ENABLED
```

### Typische Build-Probleme
1. **`byte` Fehler**: Verwende `uint8_t` statt `byte`
2. **Hängende Builds**: Terminiere pio-Prozesse bei Bedarf
3. **COM-Port Konflikte**: Stelle sicher, dass COM8 frei ist

### Testing-Workflow
1. Kompiliere mit `pio run`
2. Upload mit `pio run --target upload`
3. Monitor mit `pio device monitor`
4. Prüfe CSV-Logs auf SD-Karte auf reale Daten

## Wartung & Erweiterung

### Neue Sensoren hinzufügen
1. Pin in `config.h` definieren
2. Funktionen in `sensors.h/.cpp` implementieren
3. Non-blocking Integration in Hauptloop
4. CSV-Format in `data_logger.cpp` erweitern

### Performance-Monitoring
```cpp
// RAM-Überwachung in utilities.cpp
int freeRAM = getFreeRAM();
if (freeRAM < RAM_WARNING_THRESHOLD) {
    DEBUG_PRINTLN(F("LOW RAM WARNING"));
}
```

## Häufige Aufgaben

### Sensor-Kalibrierung
- Gas-Sensoren: 10 Sekunden Aufwärmzeit
- DHT11: State Machine für zuverlässige Initialisierung
- GPS: Kontinuierliche non-blocking Verarbeitung

### Datenformate
- **CSV-Struktur**: Timestamp,Temp,Humidity,Lat,Lon,RadCPS,MQ2-MQ9,Mic,LDR
- **Zeitstempel**: RTC-basiert im ISO-Format
- **GPS-Koordinaten**: Dezimalgrad-Format

## Kritische Erkenntnisse

1. **Niemals delay() verwenden** - zerstört GPS und Radioaktivitätsmessung
2. **State Machines für Sensor-Init** - verhindert blocking behavior
3. **CPS statt kumulativ** - Radioaktivität als Clicks Per Second messen
4. **F() Makros verwenden** - Speichert RAM durch Flash-String-Speicherung
5. **Modulare Trennung** - Jede Funktionalität in separaten Dateien
6. **PlatformIO über Arduino IDE** - Bessere Bibliotheksverwaltung

## Code-Style & Konventionen

- **Deutschen Kommentare** in Hauptcode
- **Englische Variablennamen** für Funktionen
- **Hungarian Notation** für Hardware-Pins (SENSOR_PIN)
- **Const correctness** für alle Konfigurationswerte
- **Explizite Typen** (uint8_t, uint16_t, uint32_t)

Dieses System ist produktiv getestet mit realen Sensordaten (25.8°C, 61% Luftfeuchtigkeit, GPS-Koordinaten, CPS-Radioaktivitätsmessung). Bei Änderungen die non-blocking Architektur und modulare Struktur beibehalten!
