/*
 * OLED Display Modul für das Umweltkontrollsystem
 * 0,96" SSD1306 128x64 I2C OLED Display
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

// ==============================================
// DISPLAY INITIALISIERUNG
// ==============================================

extern Adafruit_SSD1306 display;

// ==============================================
// FUNKTIONS-DEKLARATIONEN
// ==============================================

// Display Grundfunktionen
bool initDisplay();
void clearDisplay();
void updateDisplay();

// Display Seiten (rotieren alle 2 Sekunden)
void displayPage1_Status();      // System Status + Zeit
void displayPage2_Temperature(); // Temperatur + Luftfeuchtigkeit  
void displayPage3_Environment(); // Licht + Radioaktivität
void displayPage4_Gas();         // Gas-Sensoren Übersicht
void displayPage5_Audio();       // Mikrofon-Pegel
void displayPage6_GPS();         // GPS Position

// Hilfsfunktionen
void displayTitle(const char* title);
void displayValue(int line, const char* label, float value, const char* unit);
void displayText(int line, const char* text);
void nextDisplayPage();

#endif // DISPLAY_H
