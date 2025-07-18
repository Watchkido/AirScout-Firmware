/*
 * GPS-Modul für das Umweltkontrollsystem
 * Verwaltet GPS-Funktionen und Positionsdaten
 */

#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <TinyGPSPlus.h>
#include "config.h"

// ==============================================
// GPS OBJEKT
// ==============================================

extern TinyGPSPlus gpsParser;

// ==============================================
// GPS DATENSTRUKTUREN
// ==============================================

/**
 * @brief GPS-Datenstruktur mit allen relevanten Informationen
 * 
 * Diese Struktur enthält alle GPS-bezogenen Daten, die vom TinyGPSPlus
 * Parser verarbeitet werden. Sie dient als standardisierte Schnittstelle
 * für GPS-Informationen im gesamten System.
 */
struct GPSData {
  bool isValid;             ///< GPS-Signal gültig und verfügbar
  double latitude;          ///< Breitengrad in Dezimalgrad
  double longitude;         ///< Längengrad in Dezimalgrad
  double altitude;          ///< Höhe über Meeresspiegel in Metern
  double speedKmh;          ///< Geschwindigkeit in km/h
  double course;            ///< Richtung/Kurs in Grad (0-360°), 0°=Norden
  byte satellites;          ///< Anzahl der empfangenen Satelliten
  byte quality;             ///< GPS-Qualitätsindikator (HDOP-Wert)
  unsigned long lastUpdate; ///< Zeitstempel der letzten Aktualisierung (millis)
};

// ==============================================
// FUNKTIONS-DEKLARATIONEN
// ==============================================

// GPS Initialisierung
/**
 * @brief Initialisiert das GPS-Modul für das Umweltkontrollsystem.
 *
 * Diese Funktion startet die Hardware-serielle Verbindung zum GPS-Modul
 * und konfiguriert alle notwendigen Parameter. Sie muss vor allen anderen
 * GPS-Funktionen aufgerufen werden.
 *
 * @return true wenn die Initialisierung erfolgreich war, false bei Fehlern
 */
bool initGPS();

/**
 * @brief Konfiguriert die serielle Schnittstelle für GPS-Kommunikation.
 *
 * Startet Serial1 mit der in config.h definierten Baudrate für die
 * Kommunikation mit dem GPS-Modul über Hardware Serial1 des Arduino Mega.
 * Pin-Zuordnung: RX1 (Pin 19) - GPS TX, TX1 (Pin 18) - GPS RX.
 */
void setupGPSSerial();

// GPS Datenverarbeitung
/**
 * @brief Aktualisiert die GPS-Daten durch Lesen vom seriellen Port.
 *
 * Diese Funktion muss regelmäßig (idealerweise in der Hauptschleife)
 * aufgerufen werden, um eingehende GPS-NMEA-Daten zu verarbeiten.
 * Die Daten werden vom TinyGPSPlus-Parser analysiert und in internen
 * Strukturen gespeichert.
 */
void updateGPS();

/**
 * @brief Liest die aktuellen GPS-Daten und kopiert sie in die bereitgestellte Struktur.
 *
 * Diese Funktion extrahiert alle verfügbaren GPS-Informationen und berechnet
 * zusätzlich den Kurs aus Koordinatenänderungen für höhere Präzision.
 *
 * @param data Zeiger auf GPSData-Struktur, die mit aktuellen Daten gefüllt wird
 * @return true wenn gültige GPS-Daten verfügbar sind, false bei ungültigen Daten
 */
bool readGPSData(GPSData* data);

/**
 * @brief Gibt GPS-Daten formatiert über die serielle Schnittstelle aus.
 *
 * Zeigt alle wichtigen GPS-Parameter in lesbarer Form an, einschließlich
 * Koordinaten, Höhe, Geschwindigkeit, Kurs, Satellitenanzahl und Qualität.
 *
 * @param data Zeiger auf die auszugebende GPSData-Struktur
 */
void printGPSData(const GPSData* data);

// GPS Hilfsfunktionen
/**
 * @brief Berechnet den Kurs (Bearing) zwischen zwei GPS-Koordinaten.
 *
 * Verwendet die Forward-Azimuth-Formel zur präzisen Berechnung der
 * Bewegungsrichtung zwischen zwei GPS-Punkten. Dies ist genauer als
 * die direkte GPS-Kursangabe, besonders bei langsamen Bewegungen.
 *
 * @param lat1 Breitengrad des Startpunkts in Dezimalgrad
 * @param lon1 Längengrad des Startpunkts in Dezimalgrad
 * @param lat2 Breitengrad des Zielpunkts in Dezimalgrad
 * @param lon2 Längengrad des Zielpunkts in Dezimalgrad
 * @return Kurs in Grad (0-360°), wobei 0° = Norden, 90° = Osten
 */
double calculateCourse(double lat1, double lon1, double lat2, double lon2);

/**
 * @brief Berechnet die Entfernung zwischen zwei GPS-Koordinaten.
 *
 * Verwendet die Haversine-Formel zur präzisen Berechnung der
 * Großkreisentfernung auf der Erdoberfläche.
 *
 * @param lat1 Breitengrad des ersten Punkts in Dezimalgrad
 * @param lon1 Längengrad des ersten Punkts in Dezimalgrad
 * @param lat2 Breitengrad des zweiten Punkts in Dezimalgrad
 * @param lon2 Längengrad des zweiten Punkts in Dezimalgrad
 * @return Entfernung in Metern
 */
double calculateDistance(double lat1, double lon1, double lat2, double lon2);

/**
 * @brief Formatiert GPS-Koordinaten als String für die Ausgabe.
 *
 * Konvertiert Breiten- und Längengrad in einen standardisierten
 * String mit 6 Dezimalstellen Genauigkeit.
 *
 * @param lat Breitengrad in Dezimalgrad
 * @param lon Längengrad in Dezimalgrad
 * @param buffer Ausgabepuffer für den formatierten String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatCoordinates(double lat, double lon, char* buffer, byte bufferSize);

/**
 * @brief Formatiert GPS-Daten als kompakten String.
 *
 * Erstellt eine kompakte Darstellung der GPS-Koordinaten für
 * Logging oder Displayausgabe.
 *
 * @param data Zeiger auf die zu formatierenden GPSData
 * @param buffer Ausgabepuffer für den formatierten String
 * @param bufferSize Größe des Ausgabepuffers in Bytes
 */
void formatGPSString(const GPSData* data, char* buffer, byte bufferSize);

// GPS Status
/**
 * @brief Prüft, ob das GPS-Modul verbunden ist und aktuelle Daten liefert.
 *
 * Überprüft anhand des letzten Update-Zeitstempels, ob das GPS-Modul
 * noch aktiv ist. Ein Timeout von 5 Sekunden wird verwendet.
 *
 * @return true wenn GPS verbunden und aktiv, false bei Timeout oder Verbindungsproblemen
 */
bool isGPSConnected();

/**
 * @brief Gibt den Zeitstempel der letzten GPS-Datenaktualisierung zurück.
 *
 * Ermöglicht die Überwachung der GPS-Datenaktualität und die
 * Implementierung eigener Timeout-Logik.
 *
 * @return Zeitstempel in Millisekunden (millis()) der letzten GPS-Aktualisierung
 */
unsigned long getLastGPSUpdate();

/**
 * @brief Gibt die aktuelle GPS-Signalqualität zurück.
 *
 * Liefert den HDOP (Horizontal Dilution of Precision) Wert als
 * Qualitätsindikator für die GPS-Genauigkeit.
 *
 * @return GPS-Qualitätswert (niedriger = besser), 0 = keine Daten verfügbar
 */
byte getGPSQuality();

#endif // GPS_MODULE_H
