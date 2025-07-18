/*
 * RTC-Modul für das Umweltkontrollsystem
 * Verwaltet Echtzeituhren-Funktionen
 */

#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include "RTClib.h"
#include "config.h"

// ==============================================
// RTC DATENSTRUKTUREN
// ==============================================

/**
 * @brief Datenstruktur für Echtzeituhren-Informationen.
 *
 * Diese Struktur enthält alle zeitbezogenen Daten vom RTC-Modul
 * einschließlich Datum, Uhrzeit und Gültigkeitsstatus.
 */
struct RTCData {
  int year;                    ///< Jahr (4-stellig, z.B. 2025)
  byte month;                  ///< Monat (1-12)
  byte day;                    ///< Tag des Monats (1-31)
  byte hour;                   ///< Stunde (0-23)
  byte minute;                 ///< Minute (0-59)
  byte second;                 ///< Sekunde (0-59)
  unsigned long timestamp;     ///< Unix-Timestamp (Sekunden seit 1.1.1970)
  bool isValid;                ///< true wenn RTC-Daten gültig und verfügbar sind
};

// ==============================================
// RTC OBJEKT
// ==============================================

extern RTC_DS1307 rtcClock;   ///< Globales RTC-Objekt für DS1307 Echtzeituhren-Chip

// ==============================================
// FUNKTIONS-DEKLARATIONEN
// ==============================================

// RTC Initialisierung
/**
 * @brief Initialisiert das RTC-Modul (DS1307 Echtzeituhren-Chip).
 *
 * Startet die I2C-Kommunikation mit dem RTC-Chip und prüft dessen
 * Funktionsfähigkeit. Setzt bei Bedarf die Zeit auf Compile-Zeit.
 *
 * @return true wenn RTC erfolgreich initialisiert wurde, false bei Hardware-Fehlern
 */
bool initRTC();

/**
 * @brief Prüft, ob das RTC-Modul läuft und gültige Zeit liefert.
 *
 * Überprüft den internen Status des RTC-Chips und die Gültigkeit
 * der aktuellen Zeitangaben.
 *
 * @return true wenn RTC aktiv und funktionsfähig ist, false bei Problemen
 */
bool isRTCRunning();

/**
 * @brief Setzt die RTC-Zeit manuell auf die angegebenen Werte.
 *
 * Ermöglicht die manuelle Konfiguration der Echtzeituhr mit
 * spezifischen Datum- und Zeitwerten.
 *
 * @param year Jahr (4-stellig, z.B. 2025)
 * @param month Monat (1-12)
 * @param day Tag (1-31)
 * @param hour Stunde (0-23)
 * @param minute Minute (0-59)
 * @param second Sekunde (0-59)
 */
void setRTCTime(int year, byte month, byte day, byte hour, byte minute, byte second);

/**
 * @brief Setzt die RTC-Zeit auf die Compile-Zeit des Programms.
 *
 * Praktische Funktion zur automatischen Zeitsetzung basierend auf
 * dem Zeitpunkt der Programmkompilierung.
 */
void setRTCFromCompileTime();

// Zeit lesen/schreiben
/**
 * @brief Liest die aktuelle Zeit vom RTC-Modul.
 *
 * Holt die aktuellen Datum- und Zeitinformationen vom RTC-Chip
 * und füllt die bereitgestellte RTCData-Struktur.
 *
 * @param data Zeiger auf RTCData-Struktur für die gelesenen Zeitdaten
 * @return true wenn Zeit erfolgreich gelesen wurde, false bei RTC-Fehlern
 */
bool readRTCData(RTCData* data);

/**
 * @brief Gibt RTC-Daten formatiert über die serielle Schnittstelle aus.
 *
 * Zeigt Datum und Uhrzeit in benutzerfreundlichem Format an.
 *
 * @param data Zeiger auf die auszugebenden RTCData
 */
void printRTCData(const RTCData* data);

/**
 * @brief Gibt die aktuelle Zeit direkt über die serielle Schnittstelle aus.
 *
 * Liest die aktuelle Zeit vom RTC und zeigt sie sofort an,
 * ohne separate RTCData-Struktur zu benötigen.
 */
void printCurrentTime();

// GPS-Synchronisation
/**
 * @brief Synchronisiert die RTC-Zeit mit GPS-Zeitinformationen.
 *
 * Verwendet verfügbare GPS-Zeitdaten zur automatischen Korrektation
 * der Echtzeituhr für höchste Genauigkeit.
 *
 * @return true wenn Synchronisation erfolgreich war, false wenn GPS-Zeit nicht verfügbar
 */
bool syncRTCWithGPS();

/**
 * @brief Setzt die RTC-Zeit basierend auf GPS-Zeitdaten.
 *
 * Direkte Zeitsetzung mit GPS-basierten Werten zur präzisen
 * Zeitsynchronisation.
 *
 * @param year Jahr aus GPS-Daten
 * @param month Monat aus GPS-Daten
 * @param day Tag aus GPS-Daten
 * @param hour Stunde aus GPS-Daten
 * @param minute Minute aus GPS-Daten
 * @param second Sekunde aus GPS-Daten
 * @return true wenn Zeit erfolgreich gesetzt wurde, false bei Fehlern
 */
bool setRTCFromGPS(int year, int month, int day, int hour, int minute, int second);

// Zeit-Hilfsfunktionen
/**
 * @brief Formatiert Zeit als String (HH:MM:SS).
 *
 * Erstellt eine standardisierte Zeitdarstellung für Ausgaben und Logging.
 *
 * @param data Zeiger auf RTCData mit Zeitinformationen
 * @param buffer Ausgabepuffer für den formatierten Zeit-String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatTimeString(const RTCData* data, char* buffer, byte bufferSize);

/**
 * @brief Formatiert Datum als String (DD.MM.YYYY).
 *
 * Erstellt eine standardisierte Datumsdarstellung für Ausgaben und Logging.
 *
 * @param data Zeiger auf RTCData mit Datumsinformationen
 * @param buffer Ausgabepuffer für den formatierten Datum-String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatDateString(const RTCData* data, char* buffer, byte bufferSize);

/**
 * @brief Formatiert vollständigen Zeitstempel als String.
 *
 * Erstellt eine kombinierte Datum/Zeit-Darstellung für Logging
 * und Datenaufzeichnung.
 *
 * @param data Zeiger auf RTCData mit vollständigen Zeitinformationen
 * @param buffer Ausgabepuffer für den formatierten Zeitstempel
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatTimestamp(const RTCData* data, char* buffer, byte bufferSize);

// Zeitzone-Funktionen (MEZ/MESZ)
/**
 * @brief Prüft, ob ein gegebenes Datum in der Sommerzeit (MESZ) liegt.
 *
 * Berechnet anhand der europäischen Sommerzeitregeln, ob zu einem
 * bestimmten Datum die Sommerzeit aktiv ist.
 *
 * @param year Jahr zur Prüfung
 * @param month Monat zur Prüfung
 * @param day Tag zur Prüfung
 * @param hour Stunde zur Prüfung
 * @return true wenn Sommerzeit aktiv ist, false für Normalzeit
 */
bool isDST(int year, int month, int day, int hour);

/**
 * @brief Konvertiert UTC-Zeit in lokale Zeit (MEZ/MESZ).
 *
 * Passt UTC-Zeitdaten an die lokale Zeitzone (Deutschland) an,
 * berücksichtigt automatisch Sommer-/Winterzeit.
 *
 * @param data Zeiger auf RTCData-Struktur, wird modifiziert
 */
void adjustUTCToLocal(RTCData* data);

/**
 * @brief Formatiert lokale Datum/Zeit mit Zeitzone-Information.
 *
 * Erstellt einen vollständigen lokalen Zeitstempel mit
 * Zeitzone-Kennzeichnung (MEZ/MESZ).
 *
 * @param data Zeiger auf RTCData mit Zeitinformationen
 * @param buffer Ausgabepuffer für den formatierten String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatLocalDateTime(const RTCData* data, char* buffer, int bufferSize);

// Timestamp-Funktionen
/**
 * @brief Gibt den aktuellen Unix-Timestamp vom RTC zurück.
 *
 * Liest die aktuelle Zeit und konvertiert sie in einen
 * Unix-Timestamp (Sekunden seit 1.1.1970).
 *
 * @return Unix-Timestamp als unsigned long, 0 bei RTC-Fehlern
 */
unsigned long getRTCTimestamp();

/**
 * @brief Prüft die Gültigkeit von RTC-Zeitdaten.
 *
 * Validiert die Zeitdaten auf Plausibilität und korrekte Wertebereiche.
 *
 * @param data Zeiger auf zu prüfende RTCData
 * @return true wenn alle Zeitwerte gültig sind, false bei ungültigen Daten
 */
bool isTimeValid(const RTCData* data);

#endif // RTC_MODULE_H
