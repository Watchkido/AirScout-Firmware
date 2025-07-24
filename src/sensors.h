/*
 * Sensoren-Modul für das Umweltkontrollsystem
 * Verwaltet alle Sensor-Funktionen (Temperatur, Gas, Radioaktivität)
 */

#ifndef SENSORS_H
#define SENSORS_H

// #include <OneWire.h>  // DEAKTIVIERT
#include "config.h"

// ==============================================
// SENSOR INITIALISIERUNG
// ==============================================

// extern OneWire temperatureSensor;  // DEAKTIVIERT
extern bool lastRadiationState;       ///< Letzter digitaler Zustand des Radioaktivitätssensors
extern int radiationCounter;          ///< Gesamtzähler für Radioaktivitätsereignisse seit System-Start
extern unsigned long lastRadiationCheck; ///< Zeitstempel der letzten Radioaktivitätsprüfung (millis)

// ==============================================
// FUNKTIONS-DEKLARATIONEN
// ==============================================

// DHT11 Temperatur & Luftfeuchtigkeit
/**
 * @brief Initialisiert den DHT11 Temperatur- und Luftfeuchtigkeitssensor.
 *
 * Konfiguriert den DHT11-Sensor auf dem definierten Pin und führt einen
 * Funktionstest durch. Der Sensor benötigt eine kurze Aufwärmzeit.
 *
 * @return true wenn Sensor erfolgreich initialisiert und getestet wurde, false bei Fehlern
 */
bool initDHTSensor();

/**
 * @brief Liest Temperatur und Luftfeuchtigkeit vom DHT11-Sensor.
 *
 * Führt eine Messung mit dem DHT11-Sensor durch und gibt die Werte
 * über die bereitgestellten Zeiger zurück. Implementiert Retry-Logik
 * für verbesserte Zuverlässigkeit.
 *
 * @param temperature Zeiger auf Variable für Temperaturwert in °C
 * @param humidity Zeiger auf Variable für Luftfeuchtigkeit in %
 * @return true wenn Messung erfolgreich, false bei Sensor-Fehlern
 */
bool readDHTSensor(float* temperature, float* humidity);

/**
 * @brief Gibt DHT11-Messwerte formatiert über die serielle Schnittstelle aus.
 *
 * Zeigt Temperatur und Luftfeuchtigkeit in benutzerfreundlichem Format an.
 *
 * @param temperature Temperaturwert in °C
 * @param humidity Luftfeuchtigkeitswert in %
 */
void printDHTValues(float temperature, float humidity);

// Gas-Sensoren (MQ-Serie)
/**
 * @brief Initialisiert alle MQ-Serie Gassensoren.
 *
 * Konfiguriert die analogen Pins für alle 9 Gassensoren und startet
 * optional die Aufwärmphase. Die Sensoren benötigen Zeit zum Stabilisieren.
 */
void initGasSensors();

/**
 * @brief Startet die Aufwärmphase für alle Gassensoren.
 *
 * MQ-Serie Sensoren benötigen eine Aufwärmzeit von ca. 10 Sekunden
 * für stabile und genaue Messwerte.
 */
void warmupGasSensors();

/**
 * @brief Liest den Wert eines einzelnen Gassensors.
 *
 * Führt eine analoge Messung an dem angegebenen Pin durch und
 * gibt den digitalen Wert zurück.
 *
 * @param pin Analoger Pin-Nummer (A0-A8) des Gassensors
 * @return Sensorwert als 10-bit ADC-Wert (0-1023)
 */
int readGasSensor(uint8_t pin);

/**
 * @brief Liest alle 9 Gassensoren gleichzeitig.
 *
 * Führt eine vollständige Messung aller MQ-Serie Sensoren durch
 * und speichert die Werte im bereitgestellten Array.
 *
 * @param values Array mit mindestens 9 Elementen für die Sensorwerte
 */
void readAllGasSensors(int* values);

/**
 * @brief Gibt alle Gassensor-Werte formatiert aus.
 *
 * Zeigt die Werte aller 9 Gassensoren in übersichtlicher Form
 * mit Sensor-Bezeichnungen und entsprechenden Gasen.
 *
 * @param values Array mit 9 Gassensor-Werten
 */
void printGasSensorValues(int* values);

// Radioaktivitäts-Sensor
/**
 * @brief Initialisiert den Radioaktivitätssensor für hochfrequente Messung.
 *
 * Konfiguriert Pin 29 als digitalen Eingang mit Pull-up Widerstand.
 * Da dieser Pin nicht interrupt-fähig ist, muss checkRadiationSensor()
 * sehr häufig aufgerufen werden (mindestens alle 10ms für 6+ CPS).
 */
void initRadiationSensor();

/**
 * @brief Prüft den Radioaktivitätssensor auf neue Ereignisse.
 *
 * WICHTIG: Muss sehr häufig aufgerufen werden (alle 5-10ms) für
 * zuverlässige Erkennung von bis zu 6+ Clicks per Second.
 * Erkennt fallende Flanken (HIGH->LOW) als Geigerzähler-Impulse.
 */
void checkRadiationSensor();

/**
 * @brief Gibt die Gesamtanzahl der Radioaktivitätsereignisse zurück.
 *
 * Liefert die Summe aller erkannten Radioaktivitätsimpulse seit
 * dem System-Start oder dem letzten Reset.
 *
 * @return Gesamtzahl der gezählten Radioaktivitätsereignisse
 */
int getRadiationCount();

/**
 * @brief Gibt die aktuelle Impulsrate in Clicks per Second zurück.
 *
 * Berechnet die Anzahl der Radioaktivitätsereignisse in der aktuellen
 * oder letzten vollständigen Sekunde.
 *
 * @return Anzahl Clicks per Second (CPS) als Maß für Radioaktivität
 */
int getRadiationClicksPerSecond();
int getRadiationClicksPer2Seconds();

/**
 * @brief Setzt den Radioaktivitätszähler auf Null zurück.
 *
 * Löscht alle Zählerstände und startet die Messung neu.
 * Nützlich für periodische Messungen oder Kalibrierung.
 */
void resetRadiationCounter();

/**
 * @brief Gibt detaillierte Radioaktivitätsstatistiken aus.
 *
 * Zeigt umfassende Informationen über aktuelle und vergangene
 * Radioaktivitätsmessungen, einschließlich Trends und Mittelwerte.
 */
void printRadiationStats();

// Mikrofon-Sensoren
/**
 * @brief Liest den Wert eines einzelnen Mikrofon-Sensors.
 *
 * Führt eine analoge Messung des Schallpegels durch und
 * gibt den entsprechenden digitalen Wert zurück.
 *
 * @param pin Analoger Pin des Mikrofon-Sensors (A9 oder A10)
 * @return Schallpegel als 10-bit ADC-Wert (0-1023)
 */
int readMicrophone(uint8_t pin);

/**
 * @brief Liest beide Mikrofon-Sensoren gleichzeitig.
 *
 * Führt simultane Messungen beider Mikrofone durch für
 * Stereo-Schallpegelmessungen oder Richtungsbestimmung.
 *
 * @param micValues Array mit mindestens 2 Elementen für die Mikrofon-Werte
 */
void readAllMicrophones(int* micValues);

// Lichtsensor (LDR)
/**
 * @brief Liest den Rohwert des Lichtsensors (LDR).
 *
 * Misst die Lichtintensität über den lichtsensitiven Widerstand (LDR)
 * und gibt den entsprechenden ADC-Wert zurück.
 *
 * @return Lichtwert als 10-bit ADC-Wert (0-1023), höher = heller
 */
int readLightSensor();

/**
 * @brief Konvertiert den Lichtsensor-Rohwert in Prozent.
 *
 * Wandelt den ADC-Wert in einen prozentualen Helligkeitswert um
 * für benutzerfreundliche Darstellung.
 *
 * @return Helligkeit in Prozent (0-100%), 100% = maximale Helligkeit
 */
float getLightPercent();

/**
 * @brief Gibt Lichtsensor-Werte formatiert aus.
 *
 * Zeigt sowohl den Rohwert als auch den prozentualen Wert
 * der Lichtintensität in verständlicher Form.
 *
 * @param lightValue Rohwert des Lichtsensors (0-1023)
 * @param lightPercent Helligkeit in Prozent (0-100%)
 */
void printLightLevel(int lightValue, float lightPercent);

// Sensor-Diagnose
/**
 * @brief Führt einen umfassenden Test aller Sensoren durch.
 *
 * Testet alle verfügbaren Sensoren systematisch und gibt
 * Statusberichte über deren Funktionsfähigkeit aus.
 * Ideal für System-Startup und Diagnose.
 */
void testAllSensors();

/**
 * @brief Führt spezielle Tests für Temperatursensoren durch.
 *
 * Testet DHT11 und andere Temperatursensoren isoliert und
 * gibt detaillierte Diagnoseinformationen aus.
 */
void testTemperatureSensors();

#endif // SENSORS_H
