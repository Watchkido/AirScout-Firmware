/*
 * Data-Logger-Modul für das Umweltkontrollsystem
 * Verwaltet SD-Karte und Datenprotokollierung
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <SD.h>
#include "config.h"
#include "sensors.h"
#include "gps_module.h"
#include "rtc_module.h"

// ==============================================
// DATENSTRUKTUREN
// ==============================================

/**
 * @brief Vollständiger Datensatz für einen Logging-Eintrag.
 *
 * Diese Struktur enthält alle Sensordaten, GPS-Informationen und
 * Zeitstempel für einen kompletten Datenlogger-Eintrag.
 */
struct LogEntry {
  char timestamp[INDEX_LENGTH];    ///< Formatierter Zeitstempel als String

  float temperature_dht;           ///< Temperatur vom DHT11-Sensor in °C
  float humidity;                  ///< Luftfeuchtigkeit vom DHT11-Sensor in %
  GPSData gpsData;                 ///< Vollständige GPS-Informationen
  int gasSensors[9];              ///< Array mit allen 9 Gassensor-Werten (0-1023)
  int microphones[2];             ///< Array mit beiden Mikrofon-Pegeln (0-1023)
  int radiationCount;             ///< Aktueller Radioaktivitätszählerstand
  RTCData rtcData;                ///< Vollständige RTC-Zeitinformationen
};

// ==============================================
// FUNKTIONS-DEKLARATIONEN
// ==============================================

// SD-Karte Initialisierung
/**
 * @brief Initialisiert die SD-Karte für Datenlogging.
 *
 * Konfiguriert die SPI-Verbindung zur SD-Karte und prüft deren
 * Verfügbarkeit. Muss vor allen anderen SD-Karten-Operationen
 * aufgerufen werden.
 *
 * @return true wenn SD-Karte erfolgreich initialisiert wurde, false bei Hardware-Fehlern
 */
bool initSDCard();

/**
 * @brief Überprüft den aktuellen Status der SD-Karte.
 *
 * Führt eine Statusprüfung der SD-Karte durch ohne Neuinitialisierung.
 * Nützlich für periodische Verfügbarkeitsprüfungen.
 *
 * @return true wenn SD-Karte verfügbar und bereit ist, false bei Problemen
 */
bool checkSDCard();

// Datei-Management
/**
 * @brief Erstellt eine neue Log-Datei mit automatisch generiertem Namen.
 *
 * Generiert einen eindeutigen Dateinamen basierend auf aktuellem Datum/Zeit
 * und erstellt eine neue CSV-Datei für Datenlogging.
 *
 * @param filename Buffer für den generierten Dateinamen
 * @param filenameSize Größe des Filename-Buffers in Bytes
 * @return true wenn Datei erfolgreich erstellt wurde, false bei Fehlern
 */
bool createLogFile(char* filename, uint8_t filenameSize);

/**
 * @brief Öffnet eine bestehende Log-Datei zum Anhängen von Daten.
 *
 * Öffnet die angegebene Datei im Append-Modus für weitere
 * Datenprotokollierung.
 *
 * @param filename Name der zu öffnenden Log-Datei
 * @return true wenn Datei erfolgreich geöffnet wurde, false bei Fehlern
 */
bool openLogFile(const char* filename);

/**
 * @brief Schließt die aktuell geöffnete Log-Datei.
 *
 * Stellt sicher, dass alle gepufferten Daten auf die SD-Karte
 * geschrieben werden und schließt die Datei ordnungsgemäß.
 */
void closeLogFile();

/**
 * @brief Schreibt die CSV-Header-Zeile in die Log-Datei.
 *
 * Erstellt die Spaltenüberschriften für die CSV-Datei mit allen
 * Sensor- und Datenfeld-Bezeichnungen.
 *
 * @return true wenn Header erfolgreich geschrieben wurde, false bei Fehlern
 */
bool writeLogHeader();

// Datenprotokollierung
/**
 * @brief Protokolliert einen vollständigen LogEntry in die Datei.
 *
 * Schreibt alle Daten eines LogEntry-Objekts als CSV-Zeile
 * in die aktuelle Log-Datei.
 *
 * @param entry Zeiger auf den zu protokollierenden LogEntry
 * @return true wenn Daten erfolgreich geschrieben wurden, false bei Fehlern
 */
bool logData(const LogEntry* entry);

/**
 * @brief Protokolliert Sensordaten direkt ohne LogEntry-Struktur.
 *
 * Vereinfachte Logging-Funktion, die die wichtigsten Sensordaten
 * direkt entgegennimmt und als CSV-Zeile speichert.
 *
 * @param temperature_dht Temperaturwert vom DHT11-Sensor in °C
 * @param humidity Luftfeuchtigkeitswert in %
 * @param gps Zeiger auf GPS-Datenstruktur
 * @param rtc Zeiger auf RTC-Zeitdaten
 * @return true wenn Daten erfolgreich geloggt wurden, false bei Fehlern
 */
bool logSensorData(float temperature_dht, float humidity, const GPSData* gps, const RTCData* rtc);

/**
 * @brief Formatiert einen LogEntry als lesbaren String.
 *
 * Konvertiert alle LogEntry-Daten in eine strukturierte,
 * menschenlesbare Textdarstellung.
 *
 * @param entry Zeiger auf zu formatierenden LogEntry
 * @param buffer Ausgabepuffer für den formatierten String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatLogEntry(const LogEntry* entry, char* buffer, int bufferSize);

// CSV-Format Funktionen
/**
 * @brief Erstellt die CSV-Header-Zeile mit Spaltenbezeichnungen.
 *
 * Generiert eine vollständige Header-Zeile für CSV-Dateien mit
 * allen Sensor- und Datenfeld-Namen.
 *
 * @param buffer Ausgabepuffer für die Header-Zeile
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatCSVHeader(char* buffer, int bufferSize);

/**
 * @brief Formatiert einen LogEntry als CSV-Datenzeile.
 *
 * Konvertiert alle LogEntry-Daten in eine standardkonforme
 * CSV-Zeile mit Komma-Trennung.
 *
 * @param entry Zeiger auf zu formatierenden LogEntry
 * @param buffer Ausgabepuffer für die CSV-Zeile
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatCSVEntry(const LogEntry* entry, char* buffer, int bufferSize);

// Hilfsfunktionen
/**
 * @brief Generiert einen eindeutigen Dateinamen für Log-Dateien.
 *
 * Erstellt automatisch einen Dateinamen basierend auf aktuellem
 * Datum und Uhrzeit im Format "LOG-YYYY-MM-DD-HHMM.csv".
 *
 * @param filename Buffer für den generierten Dateinamen
 * @param filenameSize Größe des Filename-Buffers in Bytes
 */
void generateFilename(char* filename, uint8_t filenameSize);

/**
 * @brief Gibt Informationen über die SD-Karte aus.
 *
 * Zeigt Details wie Kartentyp, Größe, verfügbaren Speicherplatz
 * und andere technische Informationen über die serielle Schnittstelle.
 */
void printSDCardInfo();

/**
 * @brief Prüft die Verfügbarkeit der SD-Karte.
 *
 * Schnelle Überprüfung, ob die SD-Karte initialisiert und
 * für Schreiboperationen verfügbar ist.
 *
 * @return true wenn SD-Karte verfügbar ist, false wenn nicht initialisiert
 */
bool isSDCardAvailable();

// Globale Variablen
extern char globalLogFilename[FILENAME_LENGTH];  ///< Aktuell verwendeter Log-Dateiname
extern bool sdCardInitialized;                  ///< Status der SD-Karten-Initialisierung

#endif // DATA_LOGGER_H
