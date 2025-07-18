/*
 * Utilities-Modul für das Umweltkontrollsystem
 * Allgemeine Hilfsfunktionen und Tools
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>
#include "config.h"

// ==============================================
// SYSTEM-FUNKTIONEN
// ==============================================

// System-Info
/**
 * @brief Gibt umfassende Systeminformationen über die serielle Schnittstelle aus.
 *
 * Zeigt Arduino-Modell, verfügbaren RAM, Compile-Zeit, Debug-Status
 * und andere wichtige Systeminformationen für Diagnose und Überwachung.
 */
void printSystemInfo();

/**
 * @brief Gibt aktuellen Speicherverbrauch und verfügbaren RAM aus.
 *
 * Überwacht die Speichernutzung und warnt bei kritisch niedrigen
 * RAM-Werten. Wichtig für Stabilitätsüberwachung.
 */
void printMemoryUsage();

/**
 * @brief Berechnet und gibt die Menge des verfügbaren freien RAM zurück.
 *
 * Ermittelt den aktuell verfügbaren Arbeitsspeicher für
 * Speichermanagement und Optimierung.
 *
 * @return Anzahl freier RAM-Bytes
 */
unsigned int getFreeRAM();

// Reset und Watchdog
/**
 * @brief Führt einen Software-Reset des Arduino durch.
 *
 * Startet das System neu, ohne die Hardware-Reset-Taste zu betätigen.
 * Nützlich für automatische Fehlerbehebung.
 */
void softReset();

/**
 * @brief Führt umfassende Systemdiagnose und Gesundheitsprüfung durch.
 *
 * Überprüft alle kritischen Systemkomponenten (RAM, SD-Karte, RTC, GPS)
 * und gibt Statusberichte aus. Empfiehlt ggf. Neustarts.
 */
void systemCheck();

// ==============================================
// STRING-HILFSFUNKTIONEN
// ==============================================

// String-Manipulation
/**
 * @brief Entfernt führende und nachfolgende Leerzeichen aus einem String.
 *
 * Bereinigt Strings von überflüssigen Whitespace-Zeichen für
 * saubere Datenverarbeitung und Ausgabe.
 *
 * @param str Zeiger auf den zu bereinigenden String (wird modifiziert)
 */
void trimString(char* str);

/**
 * @brief Konvertiert alle Zeichen eines Strings in Kleinbuchstaben.
 *
 * Wandelt alle Großbuchstaben in dem String in Kleinbuchstaben um
 * für einheitliche Textverarbeitung.
 *
 * @param str Zeiger auf den zu konvertierenden String (wird modifiziert)
 */
void toLowerCase(char* str);

/**
 * @brief Konvertiert alle Zeichen eines Strings in Großbuchstaben.
 *
 * Wandelt alle Kleinbuchstaben in dem String in Großbuchstaben um
 * für einheitliche Textverarbeitung.
 *
 * @param str Zeiger auf den zu konvertierenden String (wird modifiziert)
 */
void toUpperCase(char* str);

// Formatierung
/**
 * @brief Formatiert einen Float-Wert als String mit definierter Dezimalstellenanzahl.
 *
 * Konvertiert Fließkommazahlen in formatierte Strings für
 * konsistente Ausgabe und Logging.
 *
 * @param value Zu formatierender Float-Wert
 * @param decimals Anzahl der Dezimalstellen
 * @param buffer Ausgabepuffer für den formatierten String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatFloat(float value, uint8_t decimals, char* buffer, uint8_t bufferSize);

/**
 * @brief Formatiert eine Ganzzahl als String mit definierter Mindestbreite.
 *
 * Konvertiert Integer-Werte in formatierte Strings mit führenden
 * Nullen oder Leerzeichen für tabellarische Ausgaben.
 *
 * @param value Zu formatierender Integer-Wert
 * @param width Mindestbreite des Ausgabestrings
 * @param buffer Ausgabepuffer für den formatierten String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatInteger(int value, uint8_t width, char* buffer, uint8_t bufferSize);

// ==============================================
// MATHEMATISCHE HILFSFUNKTIONEN
// ==============================================

// Durchschnitt und Statistik
/**
 * @brief Berechnet den Durchschnittswert eines Integer-Arrays.
 *
 * Ermittelt den arithmetischen Mittelwert einer Reihe von
 * Messwerten für statistische Auswertungen.
 *
 * @param values Zeiger auf Array mit Integer-Werten
 * @param count Anzahl der Elemente im Array
 * @return Durchschnittswert als Float
 */
float calculateAverage(int* values, uint8_t count);

/**
 * @brief Findet den kleinsten Wert in einem Integer-Array.
 *
 * Sucht das Minimum in einer Reihe von Messwerten für
 * statistische Analysen und Qualitätskontrolle.
 *
 * @param values Zeiger auf Array mit Integer-Werten
 * @param count Anzahl der Elemente im Array
 * @return Minimaler Wert im Array
 */
int findMinimum(int* values, uint8_t count);

/**
 * @brief Findet den größten Wert in einem Integer-Array.
 *
 * Sucht das Maximum in einer Reihe von Messwerten für
 * statistische Analysen und Ausreißer-Erkennung.
 *
 * @param values Zeiger auf Array mit Integer-Werten
 * @param count Anzahl der Elemente im Array
 * @return Maximaler Wert im Array
 */
int findMaximum(int* values, uint8_t count);

// Bereichsprüfungen
/**
 * @brief Prüft, ob ein Wert innerhalb eines definierten Bereichs liegt.
 *
 * Validiert Messwerte auf Plausibilität und hilft bei der
 * Erkennung von Sensor-Fehlern oder Ausreißern.
 *
 * @param value Zu prüfender Wert
 * @param min Untere Grenze des gültigen Bereichs (inklusive)
 * @param max Obere Grenze des gültigen Bereichs (inklusive)
 * @return true wenn Wert im Bereich liegt, false wenn außerhalb
 */
bool isInRange(int value, int min, int max);

/**
 * @brief Begrenzt einen Wert auf einen definierten Bereich.
 *
 * Beschränkt Werte auf gültige Bereiche und verhindert
 * Über- oder Unterschreitungen kritischer Grenzwerte.
 *
 * @param value Zu begrenzender Wert
 * @param min Untere Grenze (Minimum)
 * @param max Obere Grenze (Maximum)
 * @return Begrenzter Wert innerhalb des definierten Bereichs
 */
int constrainValue(int value, int min, int max);

// ==============================================
// TIMING-HILFSFUNKTIONEN
// ==============================================

// Non-blocking Delays
/**
 * @brief Prüft, ob ein definiertes Zeitintervall seit dem letzten Ereignis vergangen ist.
 *
 * Ermöglicht non-blocking Timing für periodische Operationen ohne
 * delay()-Funktionen. Aktualisiert automatisch den lastTime-Wert.
 *
 * @param lastTime Zeiger auf Variable mit dem letzten Zeitstempel (wird aktualisiert)
 * @param interval Zeitintervall in Millisekunden
 * @return true wenn Intervall vergangen ist, false wenn noch nicht
 */
bool isTimeElapsed(unsigned long* lastTime, unsigned long interval);

/**
 * @brief Aktualisiert einen Timer auf die aktuelle Zeit.
 *
 * Setzt einen Timer-Wert auf die aktuelle millis()-Zeit
 * für Timing-Operationen und Zeitmessungen.
 *
 * @param timer Zeiger auf zu aktualisierende Timer-Variable
 */
void updateTimer(unsigned long* timer);

/**
 * @brief Berechnet die vergangene Zeit seit einem Startzeitpunkt.
 *
 * Ermittelt die Zeitdifferenz zwischen einem gespeicherten
 * Startzeitpunkt und der aktuellen Zeit.
 *
 * @param startTime Startzeitpunkt in Millisekunden (millis()-Wert)
 * @return Vergangene Zeit in Millisekunden
 */
unsigned long getElapsedTime(unsigned long startTime);

// ==============================================
// ==============================================
// LED/STATUS-FUNKTIONEN
// ==============================================

// Status-LED (falls vorhanden)
/**
 * @brief Initialisiert eine Status-LED für Systemstatusanzeige.
 *
 * Konfiguriert einen digitalen Pin für eine Status-LED zur
 * visuellen Anzeige des Systemzustands.
 */
void initStatusLED();

/**
 * @brief Setzt den Zustand der Status-LED.
 *
 * Schaltet die Status-LED ein oder aus für einfache
 * Statusanzeigen und Debugging.
 *
 * @param state true für LED ein, false für LED aus
 */
void setStatusLED(bool state);

/**
 * @brief Lässt die Status-LED eine definierte Anzahl mal blinken.
 *
 * Erzeugt Blinkmuster für verschiedene Statuscodes und
 * Fehlermeldungen über die Status-LED.
 *
 * @param count Anzahl der Blinkvorgänge
 * @param duration Dauer jedes Blinkvorgangs in Millisekunden
 */
void blinkStatusLED(uint8_t count, unsigned int duration);

// ==============================================
// ERROR-HANDLING
// ==============================================

// Fehlerbehandlung
/**
 * @brief Aufzählung aller möglichen Systemfehler.
 *
 * Definiert standardisierte Fehlercodes für verschiedene
 * Systemkomponenten und Fehlerzustände.
 */
typedef enum {
  ERROR_NONE = 0,           ///< Kein Fehler, System funktioniert normal
  ERROR_SD_CARD = 1,        ///< SD-Karten-Fehler (nicht verfügbar, Schreibfehler)
  ERROR_RTC = 2,            ///< RTC-Fehler (Zeitmodul nicht erreichbar, Batterie leer)
  ERROR_GPS = 3,            ///< GPS-Fehler (kein Signal, Kommunikationsfehler)
  ERROR_TEMPERATURE = 4,    ///< Temperatursensor-Fehler (DHT11 antwortet nicht)
  ERROR_MEMORY = 5,         ///< Speicherfehler (zu wenig RAM verfügbar)
  ERROR_SYSTEM = 6,         ///< Allgemeiner Systemfehler (Display, unbekannte Ursache)
  ERROR_UNKNOWN = 255       ///< Unbekannter Fehler
} SystemError;

/**
 * @brief Meldet und protokolliert einen Systemfehler.
 *
 * Registriert einen Fehler im System und gibt eine entsprechende
 * Meldung über die serielle Schnittstelle aus.
 *
 * @param error Fehlercode aus der SystemError-Aufzählung
 * @param message Beschreibende Fehlermeldung als String
 */
void reportError(SystemError error, const char* message);

/**
 * @brief Löscht den aktuellen Fehlerstatus.
 *
 * Setzt den Systemfehlerstatus auf ERROR_NONE zurück
 * nach erfolgreicher Fehlerbehebung.
 */
void clearError();

/**
 * @brief Gibt den letzten aufgetretenen Systemfehler zurück.
 *
 * Ermöglicht Abfrage des aktuellen Fehlerstatus für
 * Diagnose und bedingte Programmlogik.
 *
 * @return SystemError-Code des letzten Fehlers
 */
SystemError getLastError();

// ==============================================
// KALIBRIERUNG
// ==============================================

// Sensor-Kalibrierung
/**
 * @brief Führt Kalibrierung aller Gassensoren durch.
 *
 * Kalibriert die MQ-Serie Gassensoren auf Referenzwerte
 * für verbesserte Messgenauigkeit.
 */
void calibrateGasSensors();

/**
 * @brief Kalibriert den Temperatursensor und gibt Korrekturwert zurück.
 *
 * Führt Temperaturkalibrierung durch und berechnet
 * Korrekturoffsets für präzise Messungen.
 *
 * @return Korrekturwert für Temperaturmessungen in °C
 */
float calibrateTemperatureSensor();

/**
 * @brief Speichert Kalibrierungsdaten permanent.
 *
 * Sichert alle Sensorkalibrierungswerte im EEPROM
 * für dauerhaften Erhalt nach Systemneustarts.
 */
void saveCalibrationData();

/**
 * @brief Lädt gespeicherte Kalibrierungsdaten.
 *
 * Stellt Sensorkalibrierungswerte aus dem EEPROM wieder her
 * beim Systemstart für konsistente Messgenauigkeit.
 */
void loadCalibrationData();

#endif // UTILITIES_H
